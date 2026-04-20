#include "SpanIO.h"
#include "SignalProcessing.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <functional>
#include <filesystem>
#include <system_error>
#include <cmath>        // for std::sqrt, std::pow, etc.
#include <QString>      // SensorConfig uses QString internally

namespace {

// strip trailing '\r' from CRLF lines
inline void stripCR(std::string &s)
{
    if (!s.empty() && s.back() == '\r')
        s.pop_back();
}

// parse "Key: 123" → 123
bool parseIntAfterColon(const std::string &line, int &out)
{
    const auto p = line.find(':');
    if (p == std::string::npos) return false;
    try {
        out = std::stoi(line.substr(p + 1));
        return true;
    } catch (...) {
        return false;
    }
}

// Generic "rewrite first 11 header lines and preserve the payload"
bool rewriteSpanHeader11(
    const std::string &path,
    const std::function<void(std::vector<std::string>&)> &mutateHeader)
{
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;

    std::vector<std::string> H;
    H.reserve(11);
    std::string line;

    for (int i = 0; i < 11; ++i) {
        if (!std::getline(in, line)) return false;
        stripCR(line);
        H.push_back(line);
    }
    const std::streampos payloadPos = in.tellg();

    // Let caller modify the 11 header lines in-memory
    mutateHeader(H);

    // Write to a temporary file
    const std::string tmp = path + ".tmp";
    std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
    if (!out) return false;

    for (int i = 0; i < 11; ++i) {
        out.write(H[i].data(), static_cast<std::streamsize>(H[i].size()));
        out.put('\n');
    }

    // Copy the payload (everything after line 11)
    in.clear();
    in.seekg(payloadPos);
    std::vector<char> buf(1 << 20);
    while (in) {
        in.read(buf.data(), buf.size());
        std::streamsize n = in.gcount();
        if (n > 0) out.write(buf.data(), n);
    }
    out.close();
    in.close();

    // Atomically replace the original file
    std::error_code ec;
    std::filesystem::remove(path, ec);
    std::filesystem::rename(tmp, path, ec);
    if (ec) {
        std::filesystem::copy_file(
            tmp, path,
            std::filesystem::copy_options::overwrite_existing,
            ec
            );
        std::filesystem::remove(tmp);
    }
    return true;
}

// Local data lookup: spanFile + std::string channel name
static std::vector<posData> getSignalDataFrom(const spanFile &d,
                                              const std::string &channelName)
{
    for (const auto &s : d.sensors) {
        if (s.second == channelName) {
            int idx = s.first - 1;
            if (idx >= 0 &&
                idx < static_cast<int>(d.channelsData.size())) {
                return d.channelsData[static_cast<std::size_t>(idx)];
            }
        }
    }
    return {};
}

} // anonymous namespace

namespace SpanIO {

// ----------- full .span reader ---------------------------------

bool readSpanFile(const std::string &path, spanFile &data)
{
    std::ifstream inFile(path, std::ios::binary);
    if (!inFile.is_open())
        return false;

    data.path = path;

    std::string line;
    int numWavSamples = 0;
    int numPosSamples = 0;

    // name, s1, s2, opt
    std::vector<std::tuple<std::string, std::string,
                           std::string, std::string>> derivedConfigurations;

    for (int i = 0; i < 11; ++i) {
        if (!std::getline(inFile, line))
            return false;

        stripCR(line);

        if (i == 2) {
            if (!parseIntAfterColon(line, data.numChannels))
                return false;
        } else if (i == 3) {
            if (!parseIntAfterColon(line, data.wavSR))
                return false;
        } else if (i == 4) {
            if (!parseIntAfterColon(line, data.posSR))
                return false;
        } else if (i == 6) {
            // Sensors: 1:TongueTip 2:TongueBody ...
            const auto p = line.find(':');
            if (p == std::string::npos) continue;
            std::istringstream ss(line.substr(p + 1));
            std::string sensorPair;
            data.sensors.clear();
            while (ss >> sensorPair) {
                const size_t q = sensorPair.find(':');
                if (q == std::string::npos) continue;
                int sid = 0;
                try {
                    sid = std::stoi(sensorPair.substr(0, q));
                } catch (...) {
                    continue;
                }
                data.sensors[sid] = sensorPair.substr(q + 1);
            }
        } else if (i == 7) {
            // Config: TongueTip TongueBodyx vLipy ...
            const auto p = line.find(':');
            data.configurations.clear();
            if (p == std::string::npos) continue;

            std::istringstream ss(line.substr(p + 1));
            std::string config;
            while (ss >> config) {
                QString configName = QString::fromStdString(config).trimmed();

                bool xSelected = configName.contains("x", Qt::CaseInsensitive);
                bool ySelected = configName.contains("y", Qt::CaseInsensitive);
                bool zSelected = configName.contains("z", Qt::CaseInsensitive);

                if (!xSelected && !ySelected && !zSelected) {
                    xSelected = ySelected = zSelected = true;
                }

                // motion param from prefix (optionally after 'i')
                QString motionProbe = configName.startsWith('i', Qt::CaseInsensitive)
                                          ? configName.mid(1)
                                          : configName;

                motionParam param = motionParam::Displacement;
                if (motionProbe.startsWith("v", Qt::CaseInsensitive))
                    param = motionParam::Velocity;
                else if (motionProbe.startsWith("a", Qt::CaseInsensitive))
                    param = motionParam::Acceleration;

                data.configurations.push_back(
                    SensorConfig(configName, xSelected, ySelected, zSelected, param)
                    );
            }
        } else if (i == 8) {
            // Recipe:  NAME: s1 s2 OPT; NAME2: s3 s4 OPT;
            const auto p = line.find(':');
            if (p == std::string::npos) continue;

            std::string body = line.substr(p + 1);
            if (body == " None")
                continue;

            std::istringstream ss(body);
            std::string segment;
            while (std::getline(ss, segment, ';')) {
                std::istringstream seg(segment);
                std::string name, s1, s2, opt;
                seg >> name >> s1 >> s2 >> opt;

                if (!name.empty() && name.back() == ':')
                    name.pop_back();

                if (!name.empty() && !s1.empty() && !s2.empty() && !opt.empty()) {
                    derivedConfigurations.emplace_back(name, s1, s2, opt);
                }
            }
        } else if (i == 9) {
            if (!parseIntAfterColon(line, numWavSamples))
                return false;
        } else if (i == 10) {
            if (!parseIntAfterColon(line, numPosSamples))
                return false;
        }
    }

    if (numWavSamples <= 0 || numPosSamples <= 0 ||
        data.numChannels <= 1 || data.wavSR <= 0 || data.posSR <= 0) {
        return false;
    }

    // ---- audio ----
    data.audioData.resize(numWavSamples);
    inFile.read(reinterpret_cast<char*>(data.audioData.data()),
                static_cast<std::streamsize>(numWavSamples * sizeof(float)));
    if (!inFile)
        return false;

    // ---- position channels (numChannels - 1) ----
    data.channelsData.resize(data.numChannels - 1);
    for (int j = 0; j < data.numChannels - 1; ++j) {
        auto &ch = data.channelsData[static_cast<std::size_t>(j)];
        ch.resize(numPosSamples);

        for (int i = 0; i < numPosSamples; ++i) {
            posData sample;
            inFile.read(reinterpret_cast<char*>(&sample.x),     sizeof(float));
            inFile.read(reinterpret_cast<char*>(&sample.y),     sizeof(float));
            inFile.read(reinterpret_cast<char*>(&sample.z),     sizeof(float));
            inFile.read(reinterpret_cast<char*>(&sample.phi),   sizeof(float));
            inFile.read(reinterpret_cast<char*>(&sample.theta), sizeof(float));
            inFile.read(reinterpret_cast<char*>(&sample.rms),   sizeof(float));
            ch[static_cast<std::size_t>(i)] = sample;
        }

        if (!inFile)
            return false;
    }

    inFile.close();

    // ---- interpolate positional data to wavSR ----
    for (std::size_t j = 0; j < data.channelsData.size(); ++j) {
        data.channelsData[j] =
            SignalProcessing::interpolatePositionData(data.channelsData[j],
                                                      data.posSR,
                                                      data.wavSR);
    }

    // ---- build derived channels from Recipe ----
    for (const auto &entry : derivedConfigurations) {
        const std::string &derivedName = std::get<0>(entry);
        const std::string &signal1     = std::get<1>(entry);
        const std::string &signal2     = std::get<2>(entry);
        const std::string &option      = std::get<3>(entry);

        auto sig1Data = getSignalDataFrom(data, signal1);
        auto sig2Data = getSignalDataFrom(data, signal2);
        if (sig1Data.empty() || sig2Data.empty())
            continue;

        std::vector<posData> derivedSignal;
        if (option == "SUB") {
            derivedSignal =
                SignalProcessing::subtractComponent(sig1Data, sig2Data);
        } else if (option == "DIST") {
            derivedSignal =
                SignalProcessing::computeEuclideanDistance(sig1Data, sig2Data);
        } else {
            continue;
        }

        data.channelsData.push_back(derivedSignal);
        const int newChannelIndex = static_cast<int>(data.channelsData.size());
        data.sensors[newChannelIndex] = derivedName;
    }

    return true;
}

void updateConfigLine(const std::string &path,
                      const std::vector<SensorConfig> &newConfigs)
{
    rewriteSpanHeader11(path, [&](std::vector<std::string> &H){
        std::ostringstream oss;
        oss << "Config:";
        for (const auto &cfg : newConfigs) {
            oss << ' ' << cfg.name.toStdString();
        }
        H[7] = oss.str();   // line 7 is "Config: ..."
    });
}

void overwriteRecipeLine(const std::string &path,
                         const std::string &recipeBody)
{
    rewriteSpanHeader11(path, [&](std::vector<std::string> &H){
        H[8] = "Recipe:" + recipeBody;  // line 8 is "Recipe: ..."
    });
}

std::string readRecipeBody(const std::string &path)
{
    std::ifstream in(path, std::ios::binary);
    if (!in) return " None";

    std::string line;
    for (int i = 0; i < 11 && std::getline(in, line); ++i) {
        stripCR(line);
        if (line.rfind("Recipe:", 0) == 0) {
            return line.substr(7); // text after "Recipe:"
        }
    }
    return " None";
}

void updateRecipeEntry(const std::string &spanPath,
                       const std::string &derivedName,
                       const std::string &signal1,
                       const std::string &signal2,
                       const std::string &option)
{
    rewriteSpanHeader11(spanPath, [&](std::vector<std::string> &H){
        const std::string entry =
            derivedName + ": " + signal1 + " " + signal2 + " " + option + ";";

        std::string body;
        if (H[8].rfind("Recipe:", 0) == 0)
            body = H[8].substr(7);
        if (body.empty())
            body = " None";

        if (body == " None") {
            body = " " + entry;
        } else {
            const std::string key = derivedName + ":";
            size_t pos = body.find(key);
            if (pos != std::string::npos) {
                size_t end = body.find(';', pos);
                if (end != std::string::npos) ++end;
                body.replace(pos, end - pos, entry);
            } else {
                body += " " + entry;
            }
        }
        H[8] = "Recipe:" + body;
    });
}

} // namespace SpanIO

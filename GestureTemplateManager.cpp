#include "GestureTemplateManager.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QVector>

#include <algorithm>
#include <cmath>
#include <limits>

// ---------------------------------------------------------
// ctor / basic I/O
// ---------------------------------------------------------

GestureTemplateManager::GestureTemplateManager(QObject *parent)
    : QObject(parent)
{
}

void GestureTemplateManager::setTemplateDirectory(const QString &dir)
{
    m_templateDir = dir;
    if (m_templateDir.isEmpty())
        return;

    QDir d(m_templateDir);
    if (!d.exists())
        d.mkpath(".");
}

QStringList GestureTemplateManager::availableTemplateIds() const
{
    QStringList ids;
    if (m_templateDir.isEmpty())
        return ids;

    QDir d(m_templateDir);
    if (!d.exists())
        return ids;

    const QStringList files = d.entryList(QStringList() << "*.json",
                                          QDir::Files | QDir::Readable);
    for (const QString &f : files)
        ids << QFileInfo(f).baseName();
    return ids;
}

bool GestureTemplateManager::saveTemplate(const GestureTemplate &tpl)
{
    return saveTemplateToDisk(tpl);
}

bool GestureTemplateManager::loadTemplate(const QString &templateId,
                                          GestureTemplate &out) const
{
    return loadTemplateFromDisk(templateId, out);
}

bool GestureTemplateManager::saveTemplateToDisk(const GestureTemplate &tpl) const
{
    if (m_templateDir.isEmpty())
        return false;

    QDir d(m_templateDir);
    if (!d.exists())
        d.mkpath(".");

    const QString path = d.filePath(tpl.id + ".json");

    QFile f(path);

    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QJsonObject root;
    root["id"]         = tpl.id;
    root["sampleRate"] = tpl.sampleRate;

    // We keep the original object-style "channels" map:
    //   "channels": { "TT": { ... }, "TB": { ... } }
    QJsonObject chObj;

    for (auto it = tpl.channels.constBegin(); it != tpl.channels.constEnd(); ++it) {
        const QString &chanName = it.key();
        const GestureTemplateChannelData &cd = it.value();

        QJsonObject c;
        c["idxMaxC"]  = cd.idxMaxC;
        c["cfgX"]     = cd.cfgX;
        c["cfgY"]     = cd.cfgY;
        c["cfgZ"]     = cd.cfgZ;
        c["cfgParam"] = cd.cfgParam;

        c["channel"]  = chanName;

        // Store *all* MaxC indices for this channel ---
        QJsonArray idxArr;
        for (int idx : cd.idxMaxCList)
            idxArr.append(idx);
        c["idxMaxCList"] = idxArr;

        // Samples
        QJsonArray samplesArr;
        for (double v : cd.samples)
            samplesArr.append(v);
        c["samples"] = samplesArr;

        chObj[chanName] = c;
    }

    root["channels"] = chObj;

    QJsonDocument doc(root);
    f.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

bool GestureTemplateManager::loadTemplateFromDisk(const QString &templateId,
                                                  GestureTemplate &tpl) const
{
    tpl = GestureTemplate();  // reset output

    if (m_templateDir.isEmpty())
        return false;

    QDir d(m_templateDir);
    const QString path = d.filePath(templateId + ".json");
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isObject())
        return false;

    const QJsonObject root = doc.object();
    tpl.id         = root.value("id").toString(templateId);
    tpl.sampleRate = root.value("sampleRate").toDouble(0.0);

    // "channels" is a JSON object:
    //   "channels": { "TT": { ... }, "TB": { ... } }
    const QJsonObject chObj = root.value("channels").toObject();
    for (auto it = chObj.begin(); it != chObj.end(); ++it) {
        GestureTemplateChannelData cd;

        const QJsonObject chanObj = it.value().toObject();

        cd.idxMaxC  = chanObj.value("idxMaxC").toInt(-1);
        cd.cfgX     = chanObj.value("cfgX").toBool(false);
        cd.cfgY     = chanObj.value("cfgY").toBool(false);
        cd.cfgZ     = chanObj.value("cfgZ").toBool(true);
        cd.cfgParam = chanObj.value("cfgParam").toInt(0);

        // Load list of anchors ---
        cd.idxMaxCList.clear();
        const QJsonArray idxArr = chanObj.value("idxMaxCList").toArray();
        for (const QJsonValue &v : idxArr)
            cd.idxMaxCList.append(v.toInt(-1));

        // Backwards-compatibility for templates that only have a single idxMaxC
        if (cd.idxMaxCList.isEmpty() && cd.idxMaxC >= 0)
            cd.idxMaxCList.append(cd.idxMaxC);

        // Samples
        const QJsonArray arr = chanObj.value("samples").toArray();
        cd.samples.clear();
        cd.samples.reserve(arr.size());
        for (const QJsonValue &v : arr)
            cd.samples.append(v.toDouble());

        // Key = channel name (e.g. "TT", "TTx", "vTTz", etc.)
        tpl.channels.insert(it.key(), cd);
    }

    return true;
}

// ---------------------------------------------------------
// DTW / apply helpers
// ---------------------------------------------------------

namespace {

// Same mapping you had in mainWindow.cpp
static int motionParamToInt(motionParam p)
{
    switch (p) {
    case motionParam::Velocity:     return 1;
    case motionParam::Acceleration: return 2;
    default:                        return 0;
    }
}

// Strip axis / invert / motion prefixes from config name
static inline QString baseSensorName(QString s)
{
    s = s.trimmed();
    QRegularExpression re(
        R"(^(?:i)?(?:[va])?(.+?)(?:[xyz])?$)",
        QRegularExpression::CaseInsensitiveOption
        );
    auto m = re.match(s);
    return m.hasMatch() ? m.captured(1).trimmed() : s;
}

// Find SensorConfig by name in a spanFile
static const SensorConfig *findConfigForChannel(const spanFile &data,
                                                const QString  &cfgName)
{
    for (const auto &cfg : data.configurations) {
        if (cfg.name.trimmed() == cfgName.trimmed())
            return &cfg;
    }
    return nullptr;
}

// Extract dimension-aware scalar series for THIS config
static bool extractScalarSeriesForConfig(const spanFile      &data,
                                         const SensorConfig  &cfg,
                                         QVector<double>     &out)
{
    const QString baseConfigName = baseSensorName(cfg.name);

    auto it = std::find_if(
        data.sensors.begin(), data.sensors.end(),
        [&baseConfigName, &cfg](const std::pair<int, std::string> &sensor) {
            const QString s = QString::fromStdString(sensor.second).trimmed();
            return s == baseConfigName.trimmed() || s == cfg.name.trimmed();
        });

    if (it == data.sensors.end())
        return false;

    int channelIndex = it->first - 1;
    if (channelIndex < 0 ||
        channelIndex >= static_cast<int>(data.channelsData.size()))
        return false;

    const auto &vec = data.channelsData[static_cast<size_t>(channelIndex)];
    if (vec.empty())
        return false;

    out.resize(static_cast<int>(vec.size()));

    const bool useX = cfg.x;
    const bool useY = cfg.y;
    const bool useZ = cfg.z;

    for (int i = 0; i < out.size(); ++i) {
        const auto &s = vec[static_cast<size_t>(i)];

        double sum   = 0.0;
        int    count = 0;

        if (useX) { sum += s.x; ++count; }
        if (useY) { sum += s.y; ++count; }
        if (useZ) { sum += s.z; ++count; }

        if (count > 0)
            out[i] = sum / count;
        else
            out[i] = s.x; // fallback, should basically never be hit
    }

    return true;
}

// Multi-channel DTW (unchanged from your version)
static bool dtwAlignMulti(const QVector<QVector<double>> &tpl,
                          const QVector<QVector<double>> &tok,
                          QVector<int>                   &tokenIndexForTplOut)
{
    if (tpl.isEmpty() || tok.isEmpty() || tpl.size() != tok.size())
        return false;

    const int K = tpl.size();
    const int M = tpl[0].size();
    const int N = tok[0].size();
    if (M == 0 || N == 0)
        return false;

    const double INF = 1e300;
    QVector<QVector<double>> D(N, QVector<double>(M, INF));
    QVector<QVector<char>>   back(N, QVector<char>(M, 0));

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            double dist = 0.0;
            for (int c = 0; c < K; ++c) {
                const double d = tok[c][i] - tpl[c][j];
                dist += d * d;
            }

            if (i == 0 && j == 0) {
                D[0][0]    = dist;
                back[0][0] = -1;
            } else {
                double best = INF;
                char   step = 0;

                if (i > 0 && j > 0 && D[i-1][j-1] < best) { best = D[i-1][j-1]; step = 0; }
                if (i > 0 && D[i-1][j]   < best)          { best = D[i-1][j];   step = 1; }
                if (j > 0 && D[i][j-1]   < best)          { best = D[i][j-1];   step = 2; }

                D[i][j]    = dist + best;
                back[i][j] = step;
            }
        }
    }

    // Backtrack
    int i = N - 1;
    int j = M - 1;
    QVector<QPair<int,int>> path;
    while (i >= 0 && j >= 0) {
        path.prepend(qMakePair(i, j));
        char step = back[i][j];
        if (step == -1)
            break;
        if (step == 0)      { --i; --j; }
        else if (step == 1) { --i;      }
        else if (step == 2) {      --j; }
        else break;
    }

    tokenIndexForTplOut.clear();
    tokenIndexForTplOut.resize(M, -1);
    QVector<int> counts(M, 0);

    for (const auto &p : path) {
        const int ti = p.first;
        const int tj = p.second;
        if (tj >= 0 && tj < M) {
            if (tokenIndexForTplOut[tj] < 0)
                tokenIndexForTplOut[tj] = ti;
            else
                tokenIndexForTplOut[tj] += ti;
            counts[tj]++;
        }
    }

    int last = -1;
    for (int j2 = 0; j2 < M; ++j2) {
        if (tokenIndexForTplOut[j2] >= 0) {
            if (counts[j2] > 1)
                tokenIndexForTplOut[j2] /= counts[j2];
            last = tokenIndexForTplOut[j2];
        } else if (last >= 0) {
            tokenIndexForTplOut[j2] = last;
        }
    }
    last = -1;
    for (int j2 = M - 1; j2 >= 0; --j2) {
        if (tokenIndexForTplOut[j2] >= 0) {
            last = tokenIndexForTplOut[j2];
        } else if (last >= 0) {
            tokenIndexForTplOut[j2] = last;
        }
    }

    return true;
}


static int refineMaxCUsingVelocity(const QVector<double> &disp,
                                   int idxApprox,
                                   double sampleRate)
{
    const int N = disp.size();
    if (N < 5 || sampleRate <= 0.0 || idxApprox < 2 || idxApprox > N - 3)
        return idxApprox; // nothing to do / not enough context

    // 1) Build velocity series (simple backward difference)
    QVector<double> vel(N, 0.0);
    for (int i = 1; i < N; ++i)
        vel[i] = (disp[i] - disp[i - 1]) * sampleRate;

    // 2) Look ± ~100 ms around the DTW-mapped MaxC
    int halfWin = int(std::round(0.10 * sampleRate)); // 100 ms
    if (halfWin < 5)        halfWin = 5;
    if (halfWin > N / 2)    halfWin = N / 2;

    int leftBound  = std::max(1,        idxApprox - halfWin);
    int rightBound = std::min(N - 2,    idxApprox + halfWin);

    auto strongestExtremumLeft = [&](int start) -> int {
        int    bestIdx = -1;
        double bestMag = 0.0;
        for (int i = start; i >= leftBound; --i) {
            double prev = vel[i - 1];
            double cur  = vel[i];
            double next = vel[i + 1];

            bool isMax = (cur > prev && cur >= next);
            bool isMin = (cur < prev && cur <= next);
            if (!isMax && !isMin)
                continue;

            double mag = std::fabs(cur);
            if (mag > bestMag) {
                bestMag = mag;
                bestIdx = i;
            }
        }
        return bestIdx;
    };

    auto strongestExtremumRight = [&](int start) -> int {
        int    bestIdx = -1;
        double bestMag = 0.0;
        for (int i = start; i <= rightBound; ++i) {
            double prev = vel[i - 1];
            double cur  = vel[i];
            double next = vel[i + 1];

            bool isMax = (cur >= prev && cur > next);
            bool isMin = (cur <= prev && cur < next);
            if (!isMax && !isMin)
                continue;

            double mag = std::fabs(cur);
            if (mag > bestMag) {
                bestMag = mag;
                bestIdx = i;
            }
        }
        return bestIdx;
    };

    // strongest closing/opening peaks around the DTW-mapped MaxC
    int idxP1 = strongestExtremumLeft (idxApprox - 1);
    int idxP2 = strongestExtremumRight(idxApprox + 1);
    if (idxP1 < 0 || idxP2 < 0)
        return idxApprox;  // cannot find both sides robustly

    if (idxP2 < idxP1)
        std::swap(idxP1, idxP2);

    // 3) Find near-zero velocity between P1 and P2
    int    idxZero   = -1;
    double bestAbsV  = std::numeric_limits<double>::max();
    for (int i = idxP1 + 1; i <= idxP2 - 1; ++i) {
        double a = std::fabs(vel[i]);
        if (a < bestAbsV) {
            bestAbsV = a;
            idxZero  = i;
        }
    }
    if (idxZero < 0)
        return idxApprox;

    // 4) Decide if MaxC should be a max or min from local displacement shape
    double yPrev = disp[idxApprox - 1];
    double y0    = disp[idxApprox];
    double yNext = disp[idxApprox + 1];

    bool wantMax;
    if (y0 >= yPrev && y0 >= yNext) {
        wantMax = true;
    } else if (y0 <= yPrev && y0 <= yNext) {
        wantMax = false;
    } else {
        // ambiguous: choose the direction with the larger excursion
        double up   = std::max(yPrev, yNext) - y0;
        double down = y0 - std::min(yPrev, yNext);
        wantMax = (down >= up);
    }

    // 5) Around idxZero, snap to best displacement extremum of the chosen type
    int dispHalfWin = int(std::round(0.03 * sampleRate)); // ~30 ms
    if (dispHalfWin < 2) dispHalfWin = 2;

    int dispLeft  = std::max(1,       idxZero - dispHalfWin);
    int dispRight = std::min(N - 2,   idxZero + dispHalfWin);
    if (dispLeft > dispRight)
        return idxApprox;

    int bestIdxDisp = -1;
    if (wantMax) {
        double bestVal = -std::numeric_limits<double>::max();
        for (int i = dispLeft; i <= dispRight; ++i) {
            double prev = disp[i - 1];
            double cur  = disp[i];
            double next = disp[i + 1];
            bool isMax  = (cur >= prev && cur >= next);
            if (!isMax) continue;
            if (cur > bestVal) {
                bestVal    = cur;
                bestIdxDisp = i;
            }
        }
    } else {
        double bestVal = std::numeric_limits<double>::max();
        for (int i = dispLeft; i <= dispRight; ++i) {
            double prev = disp[i - 1];
            double cur  = disp[i];
            double next = disp[i + 1];
            bool isMin  = (cur <= prev && cur <= next);
            if (!isMin) continue;
            if (cur < bestVal) {
                bestVal    = cur;
                bestIdxDisp = i;
            }
        }
    }

    if (bestIdxDisp < 0)
        return idxApprox;

    return bestIdxDisp;
}


} // namespace

// ---------------------------------------------------------
// Public DTW / apply API
// ---------------------------------------------------------

GestureTemplateManager::ApplyResult
GestureTemplateManager::applyTemplateToSpan(const spanFile        &file,
                                            const GestureTemplate &tpl,
                                            int                    uiRateHz,
                                            int                    uiMaxLen) const
{
    ApplyResult result;

    if (tpl.channels.isEmpty()) {
        result.error = tr("Template \"%1\" contains no channels.")
        .arg(tpl.id);
        return result;
    }

    // ---- Choose effective SR for the scalar series / time axis ----
    // Prefer wavSR (the one used for plotting); fall back to posSR if no audio.
    double fileTimeSR = 0.0;
    if (file.wavSR > 0.0)
        fileTimeSR = double(file.wavSR);
    else if (file.posSR > 0.0)
        fileTimeSR = double(file.posSR);

    if (tpl.sampleRate <= 0.0 || fileTimeSR <= 0.0) {
        result.error = tr("Invalid sampling rate: template SR = %1 Hz, file SR = %2 Hz.")
        .arg(tpl.sampleRate)
            .arg(fileTimeSR);
        return result;
    }

    // Require that the template and the current file are on the same time grid.
    if (std::fabs(tpl.sampleRate - fileTimeSR) > 1e-3) {
        result.error = tr("Sampling rate mismatch:\n"
                          "template SR = %1 Hz, file SR = %2 Hz.\n"
                          "Re-save the template from a file that uses the same sampling rate.")
                           .arg(tpl.sampleRate)
                           .arg(fileTimeSR);
        return result;
    }

    const double sr = fileTimeSR;   // use this SR consistently below

    // ---- Build full-resolution multi-channel signals for DTW ----
    QVector<QVector<double>> tplMultiFull;
    QVector<QVector<double>> tokMultiFull;
    QVector<QString>         channelOrder;

    int maxLenFull = 0;

    for (auto it = tpl.channels.constBegin(); it != tpl.channels.constEnd(); ++it) {
        const QString &chanName = it.key();
        const GestureTemplateChannelData &cdTpl = it.value();

        const SensorConfig *cfgNow = findConfigForChannel(file, chanName);
        if (!cfgNow) {
            result.error = tr("Channel \"%1\" is in the template but not configured "
                              "in the current file.")
                               .arg(chanName);
            return result;
        }

        if (cfgNow->x != cdTpl.cfgX ||
            cfgNow->y != cdTpl.cfgY ||
            cfgNow->z != cdTpl.cfgZ ||
            motionParamToInt(cfgNow->param) != cdTpl.cfgParam)
        {
            result.error = tr("Channel \"%1\" configuration does not match template.\n"
                              "Re-configure channels to match the exemplar.")
                               .arg(chanName);
            return result;
        }

        QVector<double> sigToken;
        if (!extractScalarSeriesForConfig(file, *cfgNow, sigToken)) {
            result.error = tr("Could not find data for channel \"%1\" in current file.")
            .arg(chanName);
            return result;
        }

        if (cdTpl.samples.isEmpty() || sigToken.isEmpty()) {
            result.error = tr("Channel \"%1\" has empty data in template or current file.")
            .arg(chanName);
            return result;
        }

        tplMultiFull.append(cdTpl.samples);
        tokMultiFull.append(sigToken);
        channelOrder.append(chanName);

        maxLenFull = std::max(maxLenFull, int(cdTpl.samples.size()));
        maxLenFull = std::max(maxLenFull, int(sigToken.size()));
    }

    if (tplMultiFull.isEmpty()) {
        result.error = tr("No valid channels to align in template \"%1\".")
        .arg(tpl.id);
        return result;
    }

    // ---- Decimate for DTW ----
    const double kTargetDtwRateHz =
        std::max(10.0, static_cast<double>(uiRateHz));
    const int kMaxDtwLen =
        std::max(200, uiMaxLen);

    int decFactor = std::max(
        1,
        static_cast<int>(std::lround(sr / kTargetDtwRateHz))
        );

    int maxLenDec = (maxLenFull + decFactor - 1) / decFactor;

    if (maxLenDec > kMaxDtwLen) {
        const int extra = (maxLenDec + kMaxDtwLen - 1) / kMaxDtwLen;
        decFactor *= extra;
        maxLenDec  = (maxLenFull + decFactor - 1) / decFactor;
    }

    auto decimate = [decFactor](const QVector<double> &in) -> QVector<double> {
        if (decFactor <= 1)
            return in;
        QVector<double> out;
        out.reserve((in.size() + decFactor - 1) / decFactor);
        for (int idx = 0; idx < in.size(); idx += decFactor)
            out.append(in[idx]);
        return out;
    };

    QVector<QVector<double>> tplMultiDec;
    QVector<QVector<double>> tokMultiDec;
    tplMultiDec.reserve(tplMultiFull.size());
    tokMultiDec.reserve(tokMultiFull.size());

    for (int c = 0; c < tplMultiFull.size(); ++c) {
        tplMultiDec.append(decimate(tplMultiFull[c]));
        tokMultiDec.append(decimate(tokMultiFull[c]));
    }

    if (tplMultiDec.isEmpty() || tplMultiDec[0].isEmpty()) {
        result.error = tr("Decimated template data is empty for \"%1\".")
        .arg(tpl.id);
        return result;
    }

    const int Mdec = tplMultiDec[0].size();
    QVector<int> mapTplToTokDec;
    if (!dtwAlignMulti(tplMultiDec, tokMultiDec, mapTplToTokDec) ||
        mapTplToTokDec.size() != Mdec)
    {
        result.error = tr("Dynamic time warping failed for template \"%1\".")
        .arg(tpl.id);
        return result;
    }

    // ---- Map template MaxC indices to token indices ----
    result.timesByChannel.clear();
    result.appliedChannels = 0;

    for (int c = 0; c < channelOrder.size(); ++c) {
        const QString &chanName = channelOrder[c];
        const GestureTemplateChannelData &cdTpl = tpl.channels[chanName];

        QVector<double> timesThisChannel;

        // NEW: loop through all anchors for this channel
        for (int jMaxC_full : cdTpl.idxMaxCList) {
            if (jMaxC_full < 0)
                jMaxC_full = 0;

            if (jMaxC_full >= tplMultiFull[c].size())
                jMaxC_full = tplMultiFull[c].size() - 1;

            int jMaxC_dec = jMaxC_full / decFactor;
            if (jMaxC_dec < 0)
                jMaxC_dec = 0;
            if (jMaxC_dec >= mapTplToTokDec.size())
                jMaxC_dec = mapTplToTokDec.size() - 1;

            const int iMaxC_dec = mapTplToTokDec[jMaxC_dec];
            if (iMaxC_dec < 0)
                continue;   // this anchor doesn’t map well, skip it

            int iMaxC_full = iMaxC_dec * decFactor;
            if (iMaxC_full >= tokMultiFull[c].size())
                iMaxC_full = tokMultiFull[c].size() - 1;

            const int iMaxC_refined =
                refineMaxCUsingVelocity(tokMultiFull[c], iMaxC_full, sr);

            const double tTokenMaxC = double(iMaxC_refined) / sr;
            timesThisChannel.append(tTokenMaxC);
            ++result.appliedChannels;
        }

        if (!timesThisChannel.isEmpty()) {
            // Optional: sort & unique to avoid duplicates
            std::sort(timesThisChannel.begin(), timesThisChannel.end());
            timesThisChannel.erase(
                std::unique(timesThisChannel.begin(), timesThisChannel.end()),
                timesThisChannel.end()
                );

            result.timesByChannel.insert(chanName, timesThisChannel);
        }
    }

    if (result.appliedChannels == 0 || result.timesByChannel.isEmpty()) {
        result.error = tr("Template \"%1\" did not result in any landmarks.")
        .arg(tpl.id);
    }

    return result;
}

#include "FileInfoDialog.h"
#include "ui_FileInfoDialog.h"

#include <QFileInfo>
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QTextCursor>
#include <QStringList>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QRegularExpression>

// --- Helper: read "Recipe:" body from the span header (first 11 lines) ---
static QString readRecipeBody(const std::string &path)
{
    QFile f(QString::fromStdString(path));
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();

    QTextStream in(&f);
    for (int i = 0; i < 11 && !in.atEnd(); ++i) {
        QString line = in.readLine().trimmed();
        if (line.startsWith("Recipe:", Qt::CaseInsensitive)) {
            // Strip the "Recipe:" prefix
            QString body = line.mid(QString("Recipe:").size()).trimmed();
            return body;
        }
    }
    return QString();
}

// --- ctor / dtor ---
FileInfoDialog::FileInfoDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FileInfoDialog)
{
    ui->setupUi(this);

    // Make sure the text edit is read-only
    ui->infoTextEdit->setReadOnly(true);

    // 1) Tell the text edit it is allowed to grow
    ui->infoTextEdit->setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Expanding);

    // 2) Ensure the dialog has a layout that contains the text edit
    //    (if you didn’t set one in Designer, layout() will be nullptr)
    if (!layout()) {
        auto *v = new QVBoxLayout(this);
        v->setContentsMargins(0, 0, 0, 0);
        v->addWidget(ui->infoTextEdit);
        setLayout(v);
    }

    // Optional: nicer default size
    resize(400, 500);
}

FileInfoDialog::~FileInfoDialog()
{
    delete ui;
}

// --- Main API ---
void FileInfoDialog::setSpanFile(const spanFile &file)
{
    QFileInfo fi(QString::fromStdString(file.path));

    const QString fileName     = fi.fileName();
    const QString filePath     = fi.absoluteFilePath();
    const QString lastModified =
        fi.lastModified().toString("yyyy-MM-dd hh:mm");

    const int audioSR = file.wavSR;
    const int posSR   = file.posSR;

    const int audioSamples =
        static_cast<int>(file.audioData.size());
    const int posSamples =
        (!file.channelsData.empty()
             ? static_cast<int>(file.channelsData[0].size())
             : 0);

    double durationSec = 0.0;
    if (audioSR > 0 && audioSamples > 0) {
        durationSec = double(audioSamples) / double(audioSR);
    }

    // Count original vs. derived channels based on id
    int nonAudioSensorCount = 0;
    int derivedCount        = 0;
    QStringList channelLines;

    for (const auto &s : file.sensors) {
        const int id = s.first;
        const QString name = QString::fromStdString(s.second).trimmed();

        bool isAudio =
            (id == 0) ||
            name.compare("audio", Qt::CaseInsensitive) == 0;
        bool isDerived = (id > file.numChannels - 1);

        if (!isAudio && !isDerived) ++nonAudioSensorCount;
        if (isDerived) ++derivedCount;

        QString flags;
        if (isAudio)   flags += "[audio] ";
        if (isDerived) flags += "[derived] ";

        channelLines << QString("  %1: %2 %3")
                            .arg(id)
                            .arg(name)
                            .arg(flags.trimmed());
    }

    // ---------- Build the info text ----------
    QString text;

    // Basic file info
    text += QString("File name: %1\n").arg(fileName);
    text += QString("File path: %1\n").arg(filePath);
    text += QString("Last modified: %1\n").arg(lastModified);
    text += "\n";

    text += QString("Duration: %1 s\n")
                .arg(durationSec, 0, 'f', 3);
    text += QString("Audio SR: %1 Hz\n").arg(audioSR);
    text += QString("POS SR: %1 Hz\n").arg(posSR);
    text += QString("Audio samples: %1\n").arg(audioSamples);
    text += QString("Position samples (per channel): %1\n").arg(posSamples);
    text += "\n";

    text += QString("Total number of channels: %1\n")
                .arg(file.numChannels);
    text += QString("EMA sensors: %1\n")
                .arg(nonAudioSensorCount);
    text += QString("Derived channels: %1\n")
                .arg(derivedCount);
    text += "\n";

    // Raw sensor list
    text += "Channels (sensor map):\n";
    text += channelLines.join('\n');
    text += "\n\n";

    // ---------- Configured channels (what is plotted, dims) ----------
    text += "Configured channels:\n";

    if (file.configurations.empty()) {
        text += "  (none)\n";
    } else {
        for (const auto &cfg : file.configurations) {
            QString dims;
            if (cfg.x) dims += "X ";
            if (cfg.y) dims += "Y ";
            if (cfg.z) dims += "Z ";
            if (dims.isEmpty()) dims = "-";

            QString paramStr;
            switch (cfg.param) {
            case motionParam::Displacement:
                paramStr = "Displacement";
                break;
            case motionParam::Velocity:
                paramStr = "Velocity";
                break;
            case motionParam::Acceleration:
                paramStr = "Acceleration";
                break;
            default:
                paramStr = "Unknown";
                break;
            }

            text += QString("  %1\n     type: %2, dims: %3\n")
                        .arg(cfg.name)
                        .arg(paramStr)
                        .arg(dims.trimmed());
        }
    }

    text += "\n";

    // ---------- Derived channel recipes (Recipe line) ----------
    text += "Derived channel recipes:\n";

    const QString recipeBody = readRecipeBody(file.path);

    if (recipeBody.isEmpty() ||
        recipeBody.compare("None", Qt::CaseInsensitive) == 0 ||
        recipeBody.compare("None", Qt::CaseInsensitive) == 0 ||
        recipeBody.compare("None", Qt::CaseInsensitive) == 0 ||
        recipeBody.compare("None", Qt::CaseInsensitive) == 0) {
        // (body in your header is " None", which becomes "None" after trimming)
        text += "  (none)\n";
    } else {
        // Example body: " name: s1 s2 SUB; other: s3 s4 DIST;"
        const QStringList segments =
            recipeBody.split(';', Qt::SkipEmptyParts);

        for (const QString &segRaw : segments) {
            QString seg = segRaw.trimmed();
            if (seg.isEmpty()) continue;

            int colonPos = seg.indexOf(':');
            if (colonPos <= 0) {
                // Fallback: dump as-is
                text += "  " + seg + "\n";
                continue;
            }

            QString name = seg.left(colonPos).trimmed();   // e.g. "VEL_TT"
            QString rest = seg.mid(colonPos + 1).trimmed(); // "CH1 CH2 SUB"

            const QStringList parts =
                rest.split(QRegularExpression("\\s+"),
                           Qt::SkipEmptyParts);
            if (parts.size() < 3) {
                text += QString("  %1: %2\n").arg(name, rest);
                continue;
            }

            const QString s1  = parts[0];
            const QString s2  = parts[1];
            const QString opt = parts[2]; // "SUB" or "DIST"

            QString methodReadable;
            if (opt.compare("SUB", Qt::CaseInsensitive) == 0)
                methodReadable = "Subtract";
            else if (opt.compare("DIST", Qt::CaseInsensitive) == 0)
                methodReadable = "Euclidean distance";
            else
                methodReadable = opt;

            text += QString("  %1 = %2(%3, %4)\n")
                        .arg(name)
                        .arg(methodReadable)
                        .arg(s1)
                        .arg(s2);
        }
    }

    // ---------- Push to widget ----------
    ui->infoTextEdit->setPlainText(text);
    ui->infoTextEdit->moveCursor(QTextCursor::Start);
}

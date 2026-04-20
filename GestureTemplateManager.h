#ifndef GESTURETEMPLATEMANAGER_H
#define GESTURETEMPLATEMANAGER_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QString>
#include "spanFile.h"
#include "GestureTemplate.h"

class GestureTemplateManager : public QObject
{
    Q_OBJECT
public:
    explicit GestureTemplateManager(QObject *parent = nullptr);

    void        setTemplateDirectory(const QString &dir);
    QStringList availableTemplateIds() const;

    bool saveTemplate(const GestureTemplate &tpl);
    bool loadTemplate(const QString &templateId,
                      GestureTemplate &out) const;

    // Result of applying a template to a spanFile
    struct ApplyResult
    {
        // For each channel, all detected gesture center times (seconds)
        QMap<QString, QVector<double>> timesByChannel;

        QString error;
        int appliedChannels = 0;   // number of channels that got ≥1 gesture

        bool ok() const
        {
            return error.isEmpty()
            && appliedChannels > 0
                && !timesByChannel.isEmpty();
        }
    };

    // Core DTW + alignment logic (no UI / no Labels)
    ApplyResult applyTemplateToSpan(const spanFile &file,
                                    const GestureTemplate &tpl,
                                    int uiRateHz,
                                    int uiMaxLen) const;

private:
    // existing stuff (m_templateDir, helpers, etc.)
    QString m_templateDir;

    bool saveTemplateToDisk(const GestureTemplate &tpl) const;
    bool loadTemplateFromDisk(const QString &templateId,
                              GestureTemplate &tpl) const;
};

#endif // GESTURETEMPLATEMANAGER_H

#ifndef GESTURETEMPLATE_H
#define GESTURETEMPLATE_H

#include <QString>
#include <QMap>
#include <QVector>

struct GestureTemplateChannelData {
    QVector<double> samples;

    // NEW: multi-anchor support
    QVector<int>    idxMaxCList;    // indices of all gesture anchors for this channel

    // Optional: keep this for backward compatibility / debugging
    int             idxMaxC = -1;   // first anchor or legacy single anchor

    bool            cfgX = false;
    bool            cfgY = false;
    bool            cfgZ = false;
    int             cfgParam = 0;
};

struct GestureTemplate
{
    QString id;
    double sampleRate = 0.0;   // Hz

    // key = config name (e.g. "TTZ", "LLZ")
    QMap<QString, GestureTemplateChannelData> channels;
};

#endif // GESTURETEMPLATE_H

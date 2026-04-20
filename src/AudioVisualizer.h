#ifndef AUDIOVISUALIZER_H
#define AUDIOVISUALIZER_H

#include <QWidget>
#include <vector>
#include "qcustomplot.h"

class AudioVisualizer : public QWidget {
    Q_OBJECT

public:
    explicit AudioVisualizer(QWidget *parent = nullptr);

    void setData(const std::vector<float> &audioData);
    void visualizeAudio(const std::vector<float> &data);
    QCustomPlot* getCustomPlot() const;

private:
    std::vector<float> audioData;
    QCustomPlot *customPlot;

    void setupCustomPlot();
    void plotAudioData();

};

#endif // AUDIOVISUALIZER_H



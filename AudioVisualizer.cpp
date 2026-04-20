#include "AudioVisualizer.h"

AudioVisualizer::AudioVisualizer(QWidget *parent) : QWidget(parent), customPlot(new QCustomPlot(this)) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(customPlot);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setLayout(layout);

}

QCustomPlot* AudioVisualizer::getCustomPlot() const {
    return customPlot;
}

void AudioVisualizer::setData(const std::vector<float> &audioData) {
    this->audioData = audioData;
}

void AudioVisualizer::visualizeAudio(const std::vector<float> &data) {
    setData(data);
    setupCustomPlot();
    plotAudioData();
}

void AudioVisualizer::setupCustomPlot() {
    customPlot->clearPlottables();
    customPlot->xAxis->setLabel("Time");
    // customPlot->yAxis->setLabel("Amplitude");
    customPlot->yAxis->setTicks(false);            // Disable ticks on the y-axis
    customPlot->yAxis->setTickLabels(false);       // Disable tick labels on the y-axis
}

void AudioVisualizer::plotAudioData() {
    QVector<double> x(audioData.size()), y(audioData.size());
    for (int i = 0; i < audioData.size(); ++i) {
        x[i] = i;
        y[i] = audioData[i];
    }
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, y);
    customPlot->rescaleAxes();
    customPlot->replot();
}

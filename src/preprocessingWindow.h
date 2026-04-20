#ifndef PREPROCESSINGWINDOW_H
#define PREPROCESSINGWINDOW_H

#include "UtilityFunctions.h"

#include <QMainWindow>
#include <QItemSelection>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QMap>
#include <QVector>

#include "OptionManager.h"
#include "Sensor3DVisualizer.h"
#include "SensorData.h"
#include "Sweep.h"

#include <memory>
#include <vector>
#include <string>
#include <unordered_set>

// Forward declaration of UI class
namespace Ui {
class preprocessingWindow;
}

class preprocessingWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit preprocessingWindow(QWidget *parent = nullptr);
    ~preprocessingWindow() override;

signals:
    void closed();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // File selection button click handlers
    void pushButton2Clicked();  // biteplate POS
    void pushButton3Clicked();  // trial POS/WAV

    // Item removal button click handlers
    void removeSelectedItemsFromList3();

    // Update button states
    void updateRemoveButtonState(const QItemSelection &selected,
                                 const QItemSelection &deselected);

    // Window button click handlers
    void cancelButtonClicked();
    void startButtonClicked();
    void proceedButtonClicked();
    void backButtonClicked();

    void onCheckBoxStateChanged(int state, int index);
    void onComboBoxChanged(int index);
    void onBpRoleComboChanged();   // BPL/BPF/BPR changed
    void addMessage(const QString &message);

private:

    // Utility functions
    std::string trim(const std::string &s);
    std::vector<channel> readPosFile(const std::string &path);

    std::vector<float> readWavFile(const std::string &path);
    std::vector<float> normalizeData(const std::vector<float>& data);

    void writeSpanFile(const std::string &path,
                       const std::vector<float> &wavData,
                       std::vector<channel> &trialData,
                       const std::string &originalPosHeaderPath,
                       const QMap<int, QString> &selectedSensors);

    void assignColors(std::vector<channel>& channels);
    void assignChannelNames(bool isBP, std::vector<channel>& channels);


    void updateGraphTitle(const QString &newTitle);
    void visualizeSelectedChannels(bool isBP,
                                   const QMap<int, QString>& trialSensors,
                                   const QMap<int, QString>& bpSensors,
                                   const QMap<int, QString>& hrSensors);

    QString selectFolderPath();

    void initializeOcclusalCorrectionModel(const std::vector<channel>& bitePlaneData,
                                           const QMap<int, QString>& hrSensors,
                                           const QMap<int, QString>& bpSensors);

    std::vector<int> selectedReferenceChannelIndices() const;
    std::vector<channel> getCorrectedTrial(const std::string &path,
                                           const std::unordered_set<int> &selectedKeys);

    // Checks and overall control flow
    bool checkInput();
    void resetWindow();
    void resetSensors();

    // Header helpers
    void readOriginalPosHeader(const std::string &path,
                               std::vector<std::string> &headerLines);
    void writeHeader(std::ofstream &outFile,
                     const std::vector<std::string> &headerLines,
                     int numberOfChannels,
                     int wavSamplingFreq,
                     int posSamplingFreq,
                     const QMap<int, QString> &sensorLabels);

    // Index helpers


    // Sensors from UI state
    void updateSensorsFromCheckboxes(); // fills hrSensors + trialSensors
    void refreshBpCombos();             // rebuild candidate list for BPL/BPF/BPR
    void syncBpSensorsFromCombos();     // fills bpSensors from BPL/BPF/BPR combos

    // UI + state
    std::unique_ptr<Ui::preprocessingWindow> ui;
    QString lastOpenedDir;
    OcclusalCorrectionModel correctionModel;

    std::unique_ptr<Sensor3DVisualizer> sensorVisualizer;

    Sweep biteplate;
    std::vector<QString> trialFilesNames;
    std::vector<Sweep> trialFiles;

    QMap<int, QString> trialSensors;
    QMap<int, QString> bpSensors;
    QMap<int, QString> hrSensors;
    std::vector<channel> bpSensorData;

    std::unique_ptr<OptionManager> optionManager;

    // Per-channel widgets (16 channels)
    QVector<QCheckBox*> checkBoxes;      // main enable per channel
    QVector<QComboBox*> comboBoxes;      // channel name per channel

    // Dedicated BPL/BPF/BPR mapping combos (add to .ui)
    QComboBox *bpLeftCombo  = nullptr;   // BP-L
    QComboBox *bpFrontCombo = nullptr;   // BP-F
    QComboBox *bpRightCombo = nullptr;   // BP-R

    QMap<int, QString> previousSelections;

    QStandardItemModel *model2 = nullptr; // BP file list
    QStandardItemModel *model3 = nullptr; // trial files list

    QStringListModel *messagesModel = nullptr;
    bool proceedPressedFirstTime = false;

    int posSR = 0;
    int wavSR = 0;

    void setupTooltips();
    void showHelpDialog();
    QString buildHelpHtml() const;

};

#endif // PREPROCESSINGWINDOW_H

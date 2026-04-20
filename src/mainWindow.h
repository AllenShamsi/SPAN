#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QItemSelection>
#include <QVector>
#include <QMap>
#include <QStringList>
#include <QButtonGroup>
#include <QTimer>
#include <QElapsedTimer>
#include <QColor>

#include <atomic>
#include <vector>
#include <string>

#include "spanFile.h"
#include "label.h"
#include <portaudio.h>

class QAbstractButton;
class QCPItemRect;
class QCPGraph;
class QCustomPlot;
class QLabel;
class QListView;
class QModelIndex;
class QRadioButton;
class QScrollBar;
class QStandardItem;
class QAction;

class Sensor3DVisualizer;
class KinematicVisualizer;
class GestureTemplateManager;
class GestureTemplate;

namespace Ui { class mainWindow; }

class mainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit mainWindow(QWidget *parent = nullptr);
    ~mainWindow();

    int  getCurrentSpanWavSR();
    int  getGlobalNameCounter() const;
    int  incrementGlobalNameCounter();

    // Exposed for Label helpers (legacy 1D per-axis velocity)
    std::vector<double> getPrecomputedVelocity(const QString &legendName);

    // 2D non-directional speed cache helper (|v_xy| = sqrt(vx^2 + vy^2))
    std::vector<double> getPrecomputedSpeedXY(const QString& configOrSensorName);

private slots:
    // Files list / buttons
    void addSpanButtonClicked();
    void removeSelectedSpanFiles();
    void updateRemoveSpanFilesButtonState(const QItemSelection &selected,
                                          const QItemSelection &deselected);

    void viewSpanFile();
    void onSpanFileDoubleClicked(const QModelIndex &index);

    // Configure tab & channels
    void onConfiguredChannelSelected(const QItemSelection &selected,
                                     const QItemSelection &deselected);
    void onRadioButtonClicked(QAbstractButton *);
    void onChannelsComboBoxIndexChanged(int index);
    void moveUpButtonClicked();
    void moveDownButtonClicked();
    void removeConfigButtonClicked();
    void updateConfigButtonClicked();
    void addConfigButtonClicked();
    void configureButtonClicked();
    void applyConfigToAllButtonClicked();

    // Derived channels
    void deriveButtonClicked();
    void clearAllDerivedButtonClicked();

    // Landmarks & CSV
    void getCSVFileButtonClicked();
    void clearAllLandmarks();
    void removeSelectedLandmark();
    void onLandmarkSelectionChanged(const QItemSelection &selected,
                                    const QItemSelection &deselected);
    void onLandmarkItemChanged(QStandardItem *item);
    void updateRemoveLandmarkButtonState();
    void onLabelClicked(const QString &channelName, double offset);
    void onLabelMoved(const QString &channelName,
                      const QString &labelName,
                      double newX,
                      double oldX);
    void onLandmarkAddedFromLabel(const QString &channel,
                                  double offset,
                                  const QString &lblName);
    void placeLabelsButtonClicked();

    // Zoom/selection/scrollbar
    void zoomInButtonClicked();
    void zoomOutButtonClicked();
    void resetButtonClicked();
    void on_selectButton_clicked();
    void onRangeScrollBarValueChanged(int value);
    void updateTrackedParameter();

    // Audio
    void playSoundButtonClicked();
    void playSelectionButtonClicked();
    void stopPlayback();
    void togglePlay();

    // Misc UI
    void pushButtonClicked();
    void invertButtonClicked();
    void rebuildSpectrogramForVisibleRange();

    // File Info
    void showFileInfoDialog();

    // Templates / DTW / spectrogram controls
    void on_saveTemplateButton_clicked();
    void on_applyTemplateButton_clicked();
    void on_batchApplyTemplateButton_clicked();
    void resetToDefaults();
    void syncAudioWidthToKinematicViewport();
    void importTemplateFromFile();
    void onSpecApplyButtonClicked();
    void onSpecDefaultsButtonClicked();

private:
    // UI enable helpers
    void enableConfigureTabButtons(bool enabled);
    void enableFilesTabButtons(bool enabled);
    void setUIElementsEnabled(bool enabled);
    void setButtonsEnabled(bool enabled);
    void refreshSpanFileRow(const QString &filePath);

    // Plotting & data visuals
    bool eventFilter(QObject *obj, QEvent *ev) override;
    void visualizeSignal(spanFile &data);
    void updateComboBox();
    void updateConfiguredChannelsListView();
    void clearAllPlots();
    void updateScrollBar();
    void highlightLabelInPlot(const QString &channelName, double offset);

    void buildSpectrogramForVisualizer(KinematicVisualizer *visualizer,
                                       const spanFile &data,
                                       double tStartSeconds = 0.0,
                                       double tEndSeconds   = -1.0);

    bool m_updatingSpectrogram = false;

    // Landmark helpers
    void addLandmarkToModel(const QString &channel, double offset, const QString &lblName);
    void removeLandmarkFromModel(double x);
    void placeLandmark(const QString &channelName, double offset, const QString &labelName);

    // SPAN I/O & config lines
    void writeSPANToTextFile(const std::string &path, const spanFile &data);
    void updateConfigLine(const std::string &path, const std::vector<SensorConfig> &newConfigs);
    void overwriteRecipeLine(const std::string &path, const std::string &recipeBody);
    void updateRecipeLine(const QString &derivedName,
                          const QString &signal1,
                          const QString &signal2,
                          const QString &option);

    // Signals / math helpers
    std::vector<posData> getSignalData(const QString &channelName);
    QVector<QVector<double>> computeSpectrogram(const std::vector<float> &audioData,
                                                int samplingRate);

    double getLandmarkYValue(const QString &channelName, double offset) const;

    void updateSpectrogramColorScale();

    // Audio worker
    void playSound(const std::vector<float> &audioData, int sampleRate);
    void playSoundAsync(std::vector<float> audioData, int sampleRate);

    // 3D sensor visualization in main window
    Sensor3DVisualizer *m_sensor3DMain = nullptr;
    QMap<int, QColor>   m_sensor3DColorMap;

    QTimer        *m_sensor3dTimer      = nullptr;
    QElapsedTimer  m_playbackClock;          // time since current playback started
    double         m_playbackStartTimeSec = 0.0; // absolute start time (s) in file
    double         m_playbackDurationSec  = 0.0; // duration of current playback segment (s)

    void initSensor3DMain();
    void populateSensor3DFromSpanFile(const spanFile &file);
    void updateSensor3DAt(double timeSeconds);
    void resetSensor3DView();

    QMap<QString, QPair<QString, QString>> m_derivedParents;   // derivedName -> (parent1, parent2)
    QMap<QString, QColor>                  m_configColorCache; // configName -> color

    QColor colorForConfig(const QString &configName);

    void autoAnnotateChannelAtTime(const QString &channelName,
                                   double timeSeconds);

    void reloadTemplatesForCurrentDir();

    bool writeCurrentLandmarksToCsv(const QString &csvFilePath,
                                    QString *errorMessage,
                                    bool showSuccessMessage);

    bool loadSpanIntoUi(const QString &filePath);

    // Help
    void setupTooltips();
    void setupMenus();
    void showHelpDialog();
    QString buildHelpHtml() const;


    Ui::mainWindow *ui{nullptr};

    QStandardItemModel *spanFilesListModel{nullptr};
    QStandardItemModel *configuredChannelsModel{nullptr};
    QStandardItemModel *landmarkListModel{nullptr};

    QButtonGroup *radioButtonGroup{nullptr};

    QCPItemRect *selectionRect{nullptr};

    QStringList       fileNames;
    QVector<QString>  spanFilesNames;
    QString           lastOpenedDir;

    QVector<KinematicVisualizer*> kinematicVisualizers;
    QVector<Label*>               m_allLabels;

    // Derived caches for label/peak workflows
    // 1) Legacy per-axis velocity (keyed by config legend, e.g., "vx")
    QMap<QString, std::vector<double>> precomputedVelocity;
    // 2) New non-directional |v_xy| speed (keyed by config/sensor name)
    QMap<QString, std::vector<double>> precomputedSpeedXY;

    spanFile *currentSpan{nullptr};

    double xAxisMinLimit{0.0};
    double xAxisMaxLimit{0.0};

    int currentLandmarkCount{1};
    int globalNameCounter{1};

    // Spectrogram cache (avoids unnecessary FFT recomputation)
    bool                     m_specCacheValid      = false;
    double                   m_specCacheWindowMs   = 0.0;
    int                      m_specCacheMaxFreqHz  = 0;
    double                   m_specCacheOverlap    = 0.0;
    int                      m_specCacheSampleRate = 0;
    qint64                   m_specCacheNumSamples = 0;
    QVector<QVector<double>> m_specCacheData;

    // Audio playback state
    std::atomic<bool>      m_isPlaying{false};
    std::atomic<bool>      m_stopRequested{false};
    std::atomic<PaStream*> m_paStream{nullptr};

    GestureTemplateManager *m_templateManager = nullptr;

    QAction *m_openAction = nullptr;
    QAction *m_preprocessAction = nullptr;
    QAction *m_viewFileAction = nullptr;
    QAction *m_placeLabelsAction = nullptr;
    QAction *m_fileInfoAction = nullptr;
    QAction *m_exitAction = nullptr;

    QAction *m_playPauseAction = nullptr;
    QAction *m_playSelectionAction = nullptr;
    QAction *m_stopAction = nullptr;

    QAction *m_selectRangeAction = nullptr;
    QAction *m_zoomInAction = nullptr;
    QAction *m_zoomOutAction = nullptr;
    QAction *m_resetViewAction = nullptr;

    QAction *m_exportCsvAction = nullptr;
    QAction *m_clearAllLandmarksAction = nullptr;

    QAction *m_helpAction = nullptr;
    QAction *m_contactAction = nullptr;
    QAction *m_aboutAction = nullptr;

};

#endif // MAINWINDOW_H

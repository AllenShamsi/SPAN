// ------------------------ Project headers ------------------------
#include "mainWindow.h"
#include "ui_mainWindow.h"

#include "SignalProcessing.h"
#include "SpanIO.h"
#include "spanFile.h"
#include "preprocessingWindow.h"
#include "KinematicVisualizer.h"
#include "Sensor3DVisualizer.h"
#include "FileInfoDialog.h"
#include "GestureTemplateManager.h"

// ------------------------ Qt headers ------------------------
#include <QComboBox>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QInputDialog>
#include <QKeySequence>
#include <QMap>
#include <QMessageBox>
#include <QMouseEvent>
#include <QRegularExpression>
#include <QScrollBar>
#include <QSet>
#include <QShortcut>
#include <QTextStream>
#include <QThread>
#include <QTimer>
#include <QVector3D>
#include <QColor>
#include <QDateTime>
#include <QProgressDialog>
#include <QSignalBlocker>

#include <QAction>
#include <QDialog>
#include <QDialogButtonBox>
#include <QMenuBar>
#include <QMenu>
#include <QTextBrowser>
#include <QDesktopServices>
#include <QUrl>

// ------------------------ STL headers ------------------------
#include <algorithm>
#include <atomic>
#include <fstream>
#include <limits>
#include <cmath>
#include <thread>

// ---------------------- Constructor / Destructor ----------------------

mainWindow::mainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::mainWindow),
    spanFilesListModel(new QStandardItemModel(this)),
    configuredChannelsModel(new QStandardItemModel(this)),
    currentSpan(nullptr),
    radioButtonGroup(new QButtonGroup(this)),
    selectionRect(nullptr),
    xAxisMinLimit(0),
    xAxisMaxLimit(0),
    landmarkListModel(new QStandardItemModel(this)),
    currentLandmarkCount(1),
    globalNameCounter(1),
    m_sensor3DMain(nullptr),
    m_templateManager(new GestureTemplateManager(this))

{
    ui->setupUi(this);

    // ----- Reset to Defaults -----
    resetToDefaults();

    // ----- Annotate tab / template manager -----
    // Where to store your template files on disk
    m_templateManager->setTemplateDirectory(
        QDir::currentPath() + "/templates"
        );

    // Populate combo box with any existing templates
    ui->templateComboBox->clear();
    ui->templateComboBox->addItems(m_templateManager->availableTemplateIds());

    // Initially disabled until a .span file is loaded
    ui->saveTemplateButton->setEnabled(false);
    ui->dtwDefaultsButton->setEnabled(false);

    connect(ui->importTemplateButton, &QPushButton::clicked, this, &mainWindow::importTemplateFromFile);
    connect(ui->dtwDefaultsButton, &QPushButton::clicked, this, &mainWindow::resetToDefaults);

    // --- Keyboard Shortcuts / Actions ---
    using QS = QKeySequence;

    // Helper to make an action quickly
    auto addAct = [this](const QList<QKeySequence> &keys, auto slot) {
        QAction *a = new QAction(this);
        a->setShortcuts(keys);
        connect(a, &QAction::triggered, this, slot);
        this->addAction(a);          // works even without a menu
        return a;
    };

    // Configs
    addAct({QS(Qt::CTRL | Qt::SHIFT | Qt::Key_Up)},    [this]{ moveUpButtonClicked(); });
    addAct({QS(Qt::CTRL | Qt::SHIFT | Qt::Key_Down)},  [this]{ moveDownButtonClicked(); });
    addAct({QS(Qt::CTRL | Qt::Key_Delete)},            [this]{ removeConfigButtonClicked(); });
    addAct({QS(Qt::CTRL | Qt::SHIFT | Qt::Key_I)},     [this]{ invertButtonClicked(); });
    addAct({QS(Qt::CTRL | Qt::SHIFT | Qt::Key_A)},     [this]{ applyConfigToAllButtonClicked(); });

    // ----- Context shortcuts on specific widgets (so Delete/Enter do the right thing) -----

    // Files list: Delete = remove file(s)
    {
        auto scDel = new QShortcut(QKeySequence(Qt::Key_Delete), ui->filesListView);
        scDel->setContext(Qt::WidgetWithChildrenShortcut);
        connect(scDel, &QShortcut::activated, this, [this]{ removeSelectedSpanFiles(); });
    }

    // Landmarks list: Delete = remove selected landmark
    {
        auto scDel = new QShortcut(QKeySequence(Qt::Key_Delete), ui->landmarkListView);
        scDel->setContext(Qt::WidgetWithChildrenShortcut);
        connect(scDel, &QShortcut::activated, this, [this]{ removeSelectedLandmark(); });
    }


    // ---------- File Info ----------------

    connect(ui->infoButton, &QPushButton::clicked, this, &mainWindow::showFileInfoDialog);

    // ---------- Landmarks table ----------
    landmarkListModel->setColumnCount(4);
    landmarkListModel->setHorizontalHeaderLabels(
        QStringList() << "Name" << "Channel" << "Offset" << "Y");

    auto *lmView = ui->landmarkListView;

    lmView->setModel(landmarkListModel);
    lmView->setSelectionMode(QAbstractItemView::SingleSelection);
    lmView->setSelectionBehavior(QAbstractItemView::SelectRows);
    lmView->setEditTriggers(QAbstractItemView::DoubleClicked);
    lmView->setSizeAdjustPolicy(QAbstractItemView::AdjustIgnored);

    // ----- Columns (horizontal header) -----
    if (auto *hHeader = lmView->horizontalHeader()) {
        hHeader->setSectionResizeMode(QHeaderView::Stretch);
        hHeader->setStretchLastSection(false);
    }

    // ----- Rows (vertical header) -----
    if (auto *vHeader = lmView->verticalHeader()) {
        vHeader->setSectionResizeMode(QHeaderView::Fixed);
        vHeader->setDefaultSectionSize(20);
    }

    connect(lmView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &mainWindow::onLandmarkSelectionChanged);
    connect(lmView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &mainWindow::updateRemoveLandmarkButtonState);

    ui->removeLandmarkButton->setEnabled(false);

    // ---------- Models & basic lists ----------
    ui->configuredChannelsListView->setModel(configuredChannelsModel);

    // Files table: 3 columns: File | Type | Last modified
    spanFilesListModel->setColumnCount(3);
    spanFilesListModel->setHorizontalHeaderLabels(
        QStringList() << "File" << "Type" << "Last modified");

    auto *filesView = ui->filesListView;              // QTableView
    filesView->setModel(spanFilesListModel);
    filesView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    filesView->setSelectionBehavior(QAbstractItemView::SelectRows);
    filesView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Let the layout control the widget size; don't grow to fit contents
    filesView->setSizeAdjustPolicy(QAbstractItemView::AdjustIgnored);
    filesView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    if (auto *header = filesView->horizontalHeader()) {
        // Keep columns user-resizable
        header->setSectionResizeMode(QHeaderView::Interactive);
        header->setStretchLastSection(false);
    }

    // ----- Rows (vertical header) -----
    if (auto *vHeader = filesView->verticalHeader()) {
        vHeader->setSectionResizeMode(QHeaderView::Fixed);   // no stretching
        vHeader->setDefaultSectionSize(20);                  // pick your row height
        // or, if you prefer auto height:
        // vHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    }

    // Auto-size "File" and "Type" columns to contents once, then let user drag
    auto autoSizeFileTableColumns = [filesView]() {
        if (!filesView || !filesView->model())
            return;

        const int total = filesView->viewport()->width();
        if (total <= 0)
            return;

        // Initial proportions: File | Type | Last modified
        filesView->setColumnWidth(0, int(total * 0.50));
        filesView->setColumnWidth(1, int(total * 0.15));
        filesView->setColumnWidth(2, int(total * 0.35));
    };

    // Run once after the first layout pass
    QTimer::singleShot(0, this, autoSizeFileTableColumns);

    // Re-run whenever rows are inserted (new files added)
    connect(spanFilesListModel, &QStandardItemModel::rowsInserted,
            this,
            [autoSizeFileTableColumns](const QModelIndex &, int, int) {
                autoSizeFileTableColumns();
            });

    // Selection handling (unchanged)
    connect(ui->filesListView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &mainWindow::updateRemoveSpanFilesButtonState);


    // ---------- Buttons ----------
    connect(ui->preprocessButton,       &QPushButton::clicked, this, &mainWindow::pushButtonClicked);
    connect(ui->clearAllLandmarkButton, &QPushButton::clicked, this, &mainWindow::clearAllLandmarks);
    connect(ui->removeLandmarkButton,   &QPushButton::clicked, this, &mainWindow::removeSelectedLandmark);
    connect(ui->playSoundButton,        &QPushButton::clicked, this, &mainWindow::playSoundButtonClicked);
    connect(ui->stopSoundButton,        &QPushButton::clicked, this, &mainWindow::stopPlayback);
    connect(ui->playSelectionButton,    &QPushButton::clicked, this, &mainWindow::playSelectionButtonClicked);
    connect(ui->viewButton,             &QPushButton::clicked, this, &mainWindow::viewSpanFile);
    connect(ui->placeLabelsButton,      &QPushButton::clicked, this, &mainWindow::placeLabelsButtonClicked);
    connect(ui->filesListView,          &QAbstractItemView::doubleClicked,
                                                               this, &mainWindow::onSpanFileDoubleClicked);
    connect(ui->addSpanFilesButton,     &QPushButton::clicked, this, &mainWindow::addSpanButtonClicked);
    connect(ui->removeSpanFilesButton,  &QPushButton::clicked, this, &mainWindow::removeSelectedSpanFiles);
    connect(ui->invertButton,           &QPushButton::clicked, this, &mainWindow::invertButtonClicked);

    // ---------- Configured channels panel ----------
    ui->channelsComboBox->setEnabled(false);
    ui->configuredChannelsListView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->configuredChannelsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->configuredChannelsListView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &mainWindow::onConfiguredChannelSelected);

    radioButtonGroup->addButton(ui->displacementRB);
    radioButtonGroup->addButton(ui->velocityRB);
    radioButtonGroup->addButton(ui->accelerationRB);
    connect(radioButtonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            this, &mainWindow::onRadioButtonClicked);

    connect(ui->moveUpButton,        &QPushButton::clicked, this, &mainWindow::moveUpButtonClicked);
    connect(ui->moveDownButton,      &QPushButton::clicked, this, &mainWindow::moveDownButtonClicked);
    connect(ui->removeConfigButton,  &QPushButton::clicked, this, &mainWindow::removeConfigButtonClicked);
    connect(ui->addConfigButton,     &QPushButton::clicked, this, &mainWindow::addConfigButtonClicked);
    connect(ui->updateConfigButton,  &QPushButton::clicked, this, &mainWindow::updateConfigButtonClicked);
    connect(ui->configureButton,     &QPushButton::clicked, this, &mainWindow::configureButtonClicked);
    connect(ui->applyConfigToAllButton, &QPushButton::clicked, this, &mainWindow::applyConfigToAllButtonClicked);
    connect(ui->channelsComboBox,    QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &mainWindow::onChannelsComboBoxIndexChanged);

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // viewport resizes on window resize AND when scrollbar appears/disappears
    ui->scrollArea->viewport()->installEventFilter(this);

    // also react when scroll range changes (content height crosses threshold)
    connect(ui->scrollArea->verticalScrollBar(), &QScrollBar::rangeChanged,
            this, [this](int, int) { syncAudioWidthToKinematicViewport(); });

    // do it once after the first layout pass
    QTimer::singleShot(0, this, [this]{ syncAudioWidthToKinematicViewport(); });


    // Disabled initially
    ui->moveUpButton->setEnabled(false);
    ui->moveDownButton->setEnabled(false);
    ui->removeConfigButton->setEnabled(false);
    ui->addConfigButton->setEnabled(false);
    ui->updateConfigButton->setEnabled(false);
    ui->configureButton->setEnabled(false);
    ui->applyConfigToAllButton->setEnabled(false);

    ui->ch1ComboBox->setEnabled(false);
    ui->ch2ComboBox->setEnabled(false);
    ui->derivedChLineEdit->setPlaceholderText("Type here");
    ui->derivedChLineEdit->setEnabled(false);
    ui->deriveButton->setEnabled(false);
    ui->channelTypeComboBox->setEnabled(false);

    ui->displacementRB->setEnabled(false);
    ui->velocityRB->setEnabled(false);
    ui->accelerationRB->setEnabled(false);
    ui->X_checkBox->setEnabled(false);
    ui->Y_checkBox->setEnabled(false);
    ui->Z_checkBox->setEnabled(false);

    ui->trackXRadioButton->setChecked(false);
    ui->trackYRadioButton->setChecked(false);
    ui->trackZRadioButton->setChecked(false);
    ui->trackAutoRadioButton->setChecked(true);

    connect(ui->trackXRadioButton, &QRadioButton::toggled, this, &mainWindow::updateTrackedParameter);
    connect(ui->trackYRadioButton, &QRadioButton::toggled, this, &mainWindow::updateTrackedParameter);
    connect(ui->trackZRadioButton, &QRadioButton::toggled, this, &mainWindow::updateTrackedParameter);
    connect(ui->trackAutoRadioButton,&QRadioButton::toggled, this, &mainWindow::updateTrackedParameter);

    // Selection/zoom/reset
    connect(ui->selectButton, &QPushButton::clicked, this, &mainWindow::on_selectButton_clicked);
    connect(ui->zoomInButton, &QPushButton::clicked, this, &mainWindow::zoomInButtonClicked);
    connect(ui->zoomOutButton, &QPushButton::clicked, this, &mainWindow::zoomOutButtonClicked);
    connect(ui->resetButton, &QPushButton::clicked, this, &mainWindow::resetButtonClicked);
    connect(ui->clearAllDerivedButton, &QPushButton::clicked, this, &mainWindow::clearAllDerivedButtonClicked);
    connect(ui->getCSVFileButton, &QPushButton::clicked, this, &mainWindow::getCSVFileButtonClicked);

    // Scrollbar
    connect(ui->rangeScrollBar, &QScrollBar::valueChanged, this, &mainWindow::onRangeScrollBarValueChanged);
    ui->rangeScrollBar->setVisible(false);
    ui->rangeScrollBar->setEnabled(false);

    connect(ui->deriveButton, &QPushButton::clicked, this, &mainWindow::deriveButtonClicked);

    // Derived ops menu
    ui->channelTypeComboBox->addItem("");
    ui->channelTypeComboBox->addItem("Subtract a component");
    ui->channelTypeComboBox->addItem("Compute Euclidean distance");

    ui->fileNameLabel->setText(" ");

    enableConfigureTabButtons(false);
    enableFilesTabButtons(false);

    connect(landmarkListModel, &QStandardItemModel::itemChanged,
            this, &mainWindow::onLandmarkItemChanged);

    // ---------- Spectrogram controls ----------
    if (ui->specWindowMsSpinBox) {
        ui->specWindowMsSpinBox->setRange(5.0, 200.0);
        ui->specWindowMsSpinBox->setSingleStep(5.0);
        ui->specWindowMsSpinBox->setValue(SignalProcessing::kSpecDefaultWindowMs);
    }

    if (ui->specViewMaxHzSpinBox) {
        ui->specViewMaxHzSpinBox->setRange(500, 8000);
        ui->specViewMaxHzSpinBox->setSingleStep(500);
        ui->specViewMaxHzSpinBox->setValue(SignalProcessing::kSpecDefaultMaxFrequencyHz);
    }

    if (ui->specDynamicRangeSpinBox) {
        ui->specDynamicRangeSpinBox->setRange(5.0, 120.0);
        ui->specDynamicRangeSpinBox->setSingleStep(5.0);
        ui->specDynamicRangeSpinBox->setValue(SignalProcessing::kSpecDefaultDynRangeDb);
    }

    if (ui->specApplyButton) {
        connect(ui->specApplyButton, &QPushButton::clicked,
                this, &mainWindow::onSpecApplyButtonClicked);
    }
    if (ui->specDefaultsButton) {
        connect(ui->specDefaultsButton, &QPushButton::clicked,
                this, &mainWindow::onSpecDefaultsButtonClicked);
    }

    // ---------- Landmark threshold controls ----------
    if (ui->landmarkThresholdSpinBox) {
        ui->landmarkThresholdSpinBox->setRange(0.01, 0.50);
        ui->landmarkThresholdSpinBox->setSingleStep(0.01);
        ui->landmarkThresholdSpinBox->setValue(m_appliedVelocityThresholdFraction);
    }

    if (ui->reopeningDeadbandSpinBox) {
        ui->reopeningDeadbandSpinBox->setRange(0.00, 0.20);
        ui->reopeningDeadbandSpinBox->setSingleStep(0.01);
        ui->reopeningDeadbandSpinBox->setValue(m_appliedReopeningDeadbandFraction);
    }

    if (ui->landmarkApplyButton) {
        connect(ui->landmarkApplyButton, &QPushButton::clicked,
                this, &mainWindow::onLandmarkDetectionApplyClicked);
    }

    if (ui->landmarkDefaultsButton) {
        connect(ui->landmarkDefaultsButton, &QPushButton::clicked,
                this, &mainWindow::onLandmarkDetectionDefaultsClicked);
    }

    // 3D Visualizer
    initSensor3DMain();

    // 3D animation timer – drives Sensor3DVisualizer while audio is playing
    m_sensor3dTimer = new QTimer(this);
    m_sensor3dTimer->setInterval(33); // ~30 FPS

    connect(m_sensor3dTimer, &QTimer::timeout, this, [this]() {
        if (!currentSpan) return;
        if (m_playbackDurationSec <= 0.0) return;

        // relative time since we started this playback segment
        const double tRel = m_playbackClock.elapsed() / 1000.0;
        if (tRel < 0.0) return;

        // stop animating when the segment is done
        if (tRel > m_playbackDurationSec) {
            m_sensor3dTimer->stop();
            return;
        }

        const double tAbs = m_playbackStartTimeSec + tRel; // absolute time in the file
        updateSensor3DAt(tAbs);
    });

    setupTooltips();
    setupMenus();
    enableFilesTabButtons(false);
    enableConfigureTabButtons(false);
}


mainWindow::~mainWindow() {
    // Stop audio and wait for worker
    stopPlayback();
    while (m_isPlaying.load(std::memory_order_acquire)) {
        QThread::msleep(5);
    }
    delete currentSpan;
    currentSpan = nullptr;
    delete ui;
}

// ------------------------ small helpers / utils ------------------------

void mainWindow::syncAudioWidthToKinematicViewport()
{
    if (!ui || !ui->scrollArea || !ui->audioContainer) return;

    const int w = ui->scrollArea->viewport()->width();

    // IMPORTANT: do NOT set fixed width (it pins the window's minimum width)
    ui->audioContainer->setMinimumWidth(0);
    ui->audioContainer->setMaximumWidth(w);

    // make sure it can shrink
    ui->audioContainer->setSizePolicy(QSizePolicy::Expanding,
                                      ui->audioContainer->sizePolicy().verticalPolicy());

    ui->audioContainer->updateGeometry();
}

void mainWindow::cacheCurrentLandmarksForSession()
{
    if (!currentSpan)
        return;

    const QString key = QString::fromStdString(currentSpan->path);

    QVector<SessionLandmark> cached;
    cached.reserve(landmarkListModel->rowCount());

    for (int row = 0; row < landmarkListModel->rowCount(); ++row) {
        if (!landmarkListModel->item(row, 0) ||
            !landmarkListModel->item(row, 1) ||
            !landmarkListModel->item(row, 2))
            continue;

        bool ok = false;
        const double offset = landmarkListModel->item(row, 2)->text().toDouble(&ok);
        if (!ok)
            continue;

        SessionLandmark lm;
        lm.name    = landmarkListModel->item(row, 0)->text();
        lm.channel = landmarkListModel->item(row, 1)->text();
        lm.offset  = offset;
        cached.push_back(lm);
    }

    if (cached.isEmpty())
        m_sessionLandmarksByFile.remove(key);
    else
        m_sessionLandmarksByFile.insert(key, cached);
}

void mainWindow::restoreSessionLandmarksForCurrentSpan()
{
    if (!currentSpan)
        return;

    const QString key = QString::fromStdString(currentSpan->path);
    if (!m_sessionLandmarksByFile.contains(key))
        return;

    const QVector<SessionLandmark> cached = m_sessionLandmarksByFile.value(key);
    if (cached.isEmpty())
        return;

    for (const SessionLandmark &lm : cached) {
        for (auto *visualizer : kinematicVisualizers) {
            if (!visualizer)
                continue;

            if (visualizer->configName().trimmed() != lm.channel.trimmed())
                continue;

            if (Label *lbl = visualizer->getLabel()) {
                lbl->placeLabelAt(lm.offset, lm.name);
            }
            break;
        }
    }

    for (auto *visualizer : kinematicVisualizers) {
        if (visualizer && visualizer->getCustomPlot())
            visualizer->getCustomPlot()->replot(QCustomPlot::rpQueuedReplot);
    }

    if (landmarkListModel->rowCount() > 0)
        enableConfigureTabButtons(true);

    syncGlobalLandmarkCounterFromModel();
}

void mainWindow::syncGlobalLandmarkCounterFromModel()
{
    int maxAutoIndex = 0;
    const QRegularExpression rx(QStringLiteral("^name(\\d+)$"),
                                QRegularExpression::CaseInsensitiveOption);

    for (int row = 0; row < landmarkListModel->rowCount(); ++row) {
        if (!landmarkListModel->item(row, 0))
            continue;

        const QString name = landmarkListModel->item(row, 0)->text().trimmed();
        const QRegularExpressionMatch m = rx.match(name);
        if (!m.hasMatch())
            continue;

        bool ok = false;
        const int n = m.captured(1).toInt(&ok);
        if (ok && n > maxAutoIndex)
            maxAutoIndex = n;
    }

    globalNameCounter = maxAutoIndex + 1;
    if (globalNameCounter < 1)
        globalNameCounter = 1;
}

void mainWindow::applyLandmarkDetectionSettingsToAllLabels()
{
    for (Label *lbl : m_allLabels) {
        if (!lbl)
            continue;

        lbl->setVelocityThresholdFraction(m_appliedVelocityThresholdFraction);
        lbl->setReopeningDeadbandFraction(m_appliedReopeningDeadbandFraction);
    }
}

void mainWindow::resetToDefaults()
{
    // DTW rate spin box
    if (ui->dtwRateSpinBox) {
        ui->dtwRateSpinBox->setRange(50, 1000);   // allow 50–1000 Hz
        ui->dtwRateSpinBox->setValue(200);        // default ~5 ms resolution
    }

    // Max DTW length spin box
    if (ui->dtwMaxLenSpinBox) {
        ui->dtwMaxLenSpinBox->setRange(500, 20000);  // safety envelope
        ui->dtwMaxLenSpinBox->setValue(5000);        // default max length
    }

    // Later: if we add more user controls (checkboxes, other spin boxes, etc.),
    // just set their defaults here as well.
}

void mainWindow::reloadTemplatesForCurrentDir()
{
    ui->templateComboBox->clear();

    if (!currentSpan) {
        return;
    }

    const QString spanPath = QString::fromStdString(currentSpan->path);
    QFileInfo fi(spanPath);
    const QString templateDir = fi.absolutePath();

    // Keep GestureTemplateManager in sync with the directory we use
    if (m_templateManager) {
        m_templateManager->setTemplateDirectory(templateDir);
    }

    QDir dir(templateDir);
    const QStringList jsonFiles =
        dir.entryList(QStringList() << "*.json", QDir::Files);

    for (const QString &fileName : jsonFiles) {
        const QString base = QFileInfo(fileName).completeBaseName();
        ui->templateComboBox->addItem(base);
    }
}

// Map motionParam to small int for template
static int motionParamToInt(motionParam p)
{
    switch (p) {
    case motionParam::Velocity:     return 1;
    case motionParam::Acceleration: return 2;
    default:                        return 0;
    }
}

// Find SensorConfig by name
static const SensorConfig* findConfigForChannel(const spanFile &data,
                                                const QString &cfgName)
{
    for (const auto &cfg : data.configurations) {
        if (cfg.name.trimmed() == cfgName.trimmed())
            return &cfg;
    }
    return nullptr;
}

// Shared palette for sensors (qualitative, for white background)
QVector<QColor> kSensorPalette()
{
    static QVector<QColor> colors = {
        QColor(139,   0,   0),  // darkred
        QColor(139,  69,  19),  // saddlebrown
        QColor(107, 142,  35),  // olivedrab

        QColor(184, 134,  11),  // darkgoldenrod
        QColor(  0, 128, 128),  // teal
        QColor( 72,  61, 139),  // darkslateblue
        QColor( 85, 107,  47),  // darkolivegreen
        QColor(160,  82,  45),  // sienna
        QColor(178,  34,  34),  // firebrick
        QColor( 25,  25, 112),  // midnightblue
        QColor( 75,   0, 130),  // indigo
        QColor(105, 105, 105),  // dimgray

        QColor(165,  42,  42),  // brown
        QColor(  0, 139, 139),  // darkcyan
        QColor(  0,   0, 128),  // navy
        QColor(102,  51, 153),  // rebeccapurple
        QColor(112, 128, 144),  // slategray
        QColor(128,   0,   0),  // maroon
        QColor(  0, 100,   0),  // darkgreen
        QColor( 47,  79,  79),  // darkslategray
        QColor(  0,   0, 139),  // darkblue
        QColor(139,   0, 139),  // darkmagenta
        QColor(210, 105,  30),  // chocolate
        QColor( 46, 139,  87),  // seagreen
        QColor(148,   0, 211),  // darkviolet
        QColor(205, 133,  63),  // peru
        QColor( 70, 130, 180)   // steelblue

    };
    return colors;
}

QColor colorForIndex(int i)
{
    const auto &colors = kSensorPalette();
    if (colors.isEmpty())
        return Qt::white;

    int idx = i % colors.size();
    if (idx < 0) idx += colors.size();
    return colors[idx];
}

// Simple RGB average
QColor mixColors(const QColor &a, const QColor &b)
{
    if (!a.isValid()) return b.isValid() ? b : QColor(Qt::gray);
    if (!b.isValid()) return a;
    return QColor(
        (a.red()   + b.red())   / 2,
        (a.green() + b.green()) / 2,
        (a.blue()  + b.blue())  / 2
        );
}

int mainWindow::getCurrentSpanWavSR() {
    return currentSpan ? currentSpan->wavSR : 0;
}

int mainWindow::getGlobalNameCounter() const {
    return globalNameCounter;
}

int mainWindow::incrementGlobalNameCounter() {
    return ++globalNameCounter;
}

void mainWindow::enableConfigureTabButtons(bool enabled) {
    ui->landmarkListView->setEnabled(enabled);
    ui->removeLandmarkButton->setEnabled(enabled);
    ui->clearAllLandmarkButton->setEnabled(enabled);
    ui->getCSVFileButton->setEnabled(enabled);
}

void mainWindow::enableFilesTabButtons(bool enabled) {
    ui->removeSpanFilesButton->setEnabled(enabled);
    ui->viewButton->setEnabled(enabled);
    ui->placeLabelsButton->setEnabled(enabled);
    ui->infoButton->setEnabled(enabled);

    ui->trackXRadioButton->setEnabled(enabled);
    ui->trackYRadioButton->setEnabled(enabled);
    ui->trackZRadioButton->setEnabled(enabled);
    ui->trackAutoRadioButton->setEnabled(enabled);

    ui->playSoundButton->setEnabled(enabled);
    ui->playSelectionButton->setEnabled(enabled);
    ui->stopSoundButton->setEnabled(enabled);

    ui->zoomInButton->setEnabled(enabled);
    ui->zoomOutButton->setEnabled(enabled);
    ui->selectButton->setEnabled(enabled);
    ui->resetButton->setEnabled(enabled);

    ui->configureTab->setEnabled(enabled);
    ui->labelTab->setEnabled(enabled);

    // Annotate tab widgets
    ui->settingsTab->setEnabled(enabled);
    ui->saveTemplateButton->setEnabled(enabled && currentSpan);
    ui->dtwDefaultsButton->setEnabled(enabled && !spanFilesNames.empty());
    ui->templateComboBox->setEnabled(enabled);

    if (ui->specApplyButton)         ui->specApplyButton->setEnabled(enabled);
    if (ui->specDefaultsButton)      ui->specDefaultsButton->setEnabled(enabled);
    if (ui->specWindowMsSpinBox)     ui->specWindowMsSpinBox->setEnabled(enabled);
    if (ui->specViewMaxHzSpinBox)    ui->specViewMaxHzSpinBox->setEnabled(enabled);
    if (ui->specDynamicRangeSpinBox) ui->specDynamicRangeSpinBox->setEnabled(enabled);

    // Menu actions that should track file-loaded state
    if (m_viewFileAction)            m_viewFileAction->setEnabled(enabled);
    if (m_placeLabelsAction)         m_placeLabelsAction->setEnabled(enabled);
    if (m_fileInfoAction)            m_fileInfoAction->setEnabled(enabled);

    if (m_playPauseAction)           m_playPauseAction->setEnabled(enabled);
    if (m_playSelectionAction)       m_playSelectionAction->setEnabled(enabled);
    if (m_stopAction)                m_stopAction->setEnabled(enabled);

    if (m_selectRangeAction)         m_selectRangeAction->setEnabled(enabled);
    if (m_zoomInAction)              m_zoomInAction->setEnabled(enabled);
    if (m_zoomOutAction)             m_zoomOutAction->setEnabled(enabled);
    if (m_resetViewAction)           m_resetViewAction->setEnabled(enabled);

    if (m_exportCsvAction)           m_exportCsvAction->setEnabled(enabled);
    if (m_clearAllLandmarksAction)   m_clearAllLandmarksAction->setEnabled(enabled);

    // These remain always enabled
    if (m_openAction)                m_openAction->setEnabled(true);
    if (m_preprocessAction)          m_preprocessAction->setEnabled(true);
    if (m_exitAction)                m_exitAction->setEnabled(true);
    if (m_helpAction)                m_helpAction->setEnabled(true);
    if (m_contactAction)             m_contactAction->setEnabled(true);
    if (m_aboutAction)               m_aboutAction->setEnabled(true);
}

bool mainWindow::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == ui->scrollArea->viewport()) {
        if (ev->type() == QEvent::Resize || ev->type() == QEvent::LayoutRequest) {
            syncAudioWidthToKinematicViewport();
        }
    }
    return QMainWindow::eventFilter(obj, ev);
}

static void clearLayout(QLayout* layout)
{
    if (!layout) return;
    while (QLayoutItem* item = layout->takeAt(0)) {
        if (QWidget* w = item->widget())
            w->deleteLater();
        if (QLayout* childLayout = item->layout())
            clearLayout(childLayout);
        delete item;
    }
}

static inline QString baseSensorName(QString s)
{
    s = s.trimmed();
    if (s.isEmpty())
        return s;

    // Strip optional leading invert 'i' (case-insensitive)
    if (s.startsWith('i', Qt::CaseInsensitive))
        s.remove(0, 1);

    // Strip optional motion prefix 'v' or 'a' (velocity / acceleration)
    if (!s.isEmpty()) {
        const QChar c = s[0];
        if (c == 'v' || c == 'V' || c == 'a' || c == 'A')
            s.remove(0, 1);
    }

    // Strip up to three trailing axis chars x/y/z (e.g. x, xy, xyz)
    int removed = 0;
    while (!s.isEmpty() && removed < 3) {
        const QChar c = s.back();
        if (c == 'x' || c == 'X' ||
            c == 'y' || c == 'Y' ||
            c == 'z' || c == 'Z')
        {
            s.chop(1);
            ++removed;
        } else {
            break;
        }
    }

    return s.trimmed();
}

// Resolve configured channel name -> index in channelsData and extract a 1D scalar series
// consistent with the SensorConfig (dimension-aware).
static bool extractScalarSeriesForConfig(const spanFile &data,
                                         const SensorConfig &cfg,
                                         QVector<double> &out)
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

        // Average over whichever axes are selected for this config.
        if (useX) { sum += s.x; ++count; }
        if (useY) { sum += s.y; ++count; }
        if (useZ) { sum += s.z; ++count; }

        if (count > 0) {
            out[i] = sum / count;
        } else {
            // Fallback – shouldn't happen for valid configs,
            // but keeps behavior defined if someone creates a 0/0/0 config.
            out[i] = s.x;
        }
    }

    return true;
}


static inline bool hasInvertPrefix(const QString& s) {
    return s.startsWith('i', Qt::CaseInsensitive);
}


// ------------------------ config hygiene ------------------------

static void pruneConfigsToExistingSensors(spanFile& data)
{
    auto sensorExists = [&](const QString& cfgName) -> bool {
        const QString base = baseSensorName(cfgName);
        for (const auto& s : data.sensors) {
            const QString sname = QString::fromStdString(s.second).trimmed();
            if (sname.compare(base, Qt::CaseInsensitive) == 0 ||
                sname.compare(cfgName.trimmed(), Qt::CaseInsensitive) == 0)
                return true;
        }
        return false;
    };

    auto& cfgs = data.configurations;
    cfgs.erase(std::remove_if(cfgs.begin(), cfgs.end(),
                              [&](const SensorConfig& c){ return !sensorExists(c.name); }),
               cfgs.end());
}

// ------------------------ color config resolver ------------------------


QColor mainWindow::colorForConfig(const QString &configName)
{
    const QString trimmed = configName.trimmed();
    if (trimmed.isEmpty())
        return Qt::gray;

    // Cache first
    if (m_configColorCache.contains(trimmed))
        return m_configColorCache.value(trimmed);

    const QString base = baseSensorName(trimmed);

    // --- 1) Try to treat it as a raw sensor (non-derived) and look up its 3D color ---
    if (currentSpan) {
        for (const auto &sensor : currentSpan->sensors) {
            const QString sname = QString::fromStdString(sensor.second).trimmed();

            if (sname.compare(base, Qt::CaseInsensitive) == 0 ||
                sname.compare(trimmed, Qt::CaseInsensitive) == 0)
            {
                int sensorId = sensor.first;
                if (m_sensor3DColorMap.contains(sensorId)) {
                    const QColor c = m_sensor3DColorMap.value(sensorId);
                    m_configColorCache.insert(trimmed, c);
                    return c;
                }
            }
        }
    }

    // --- 2) Derived channel: mix colors of its parents ---
    if (m_derivedParents.contains(trimmed) || m_derivedParents.contains(base)) {
        const auto parents = m_derivedParents.value(
            trimmed,
            m_derivedParents.value(base)   // fallback to base name key
            );
        const QColor c1 = colorForConfig(parents.first);
        const QColor c2 = colorForConfig(parents.second);
        const QColor mix = ::mixColors(c1, c2);

        m_configColorCache.insert(trimmed, mix);
        return mix;
    }

    // --- 3) Fallback: deterministic but not critical (should rarely happen) ---
    static int fallbackIndex = 0;
    const QColor fallback = colorForIndex(fallbackIndex++);
    m_configColorCache.insert(trimmed, fallback);
    return fallback;
}


// ------------------------ Derived ops ------------------------

void mainWindow::updateConfigLine(const std::string &path,
                                  const std::vector<SensorConfig> &newConfigs)
{
    SpanIO::updateConfigLine(path, newConfigs);
    refreshSpanFileRow(QString::fromStdString(path));
}

void mainWindow::overwriteRecipeLine(const std::string &path,
                                     const std::string &recipeBody)
{
    SpanIO::overwriteRecipeLine(path, recipeBody);
    refreshSpanFileRow(QString::fromStdString(path));
}


void mainWindow::clearAllDerivedButtonClicked()
{
    if (!currentSpan)
        return;

    const std::string path     = currentSpan->path;
    const QString     filePath = QString::fromStdString(path);

    // 1) Clear Recipe line in the header (no more derived channels)
    overwriteRecipeLine(path, " None");

    // 2) Rebuild in-memory spanFile
    delete currentSpan;
    currentSpan = new spanFile();

    clearAllPlots();
    resetSensor3DView();              // also clear the 3D widget

    // Re-read the file
    SpanIO::readSpanFile(path, *currentSpan);

    // 3) Prune configs that reference removed sensors
    pruneConfigsToExistingSensors(*currentSpan);

    // 4) Persist cleaned Config: line
    updateConfigLine(path, currentSpan->configurations);

    // 4.5) Reset color state and repopulate 3D colors
    m_configColorCache.clear();
    m_derivedParents.clear();
    populateSensor3DFromSpanFile(*currentSpan);

    // 5) Re-draw + refresh UI
    visualizeSignal(*currentSpan);
    updateComboBox();
    updateConfiguredChannelsListView();
    updateScrollBar();

    ui->trackXRadioButton->setChecked(false);
    ui->trackYRadioButton->setChecked(false);
    ui->trackZRadioButton->setChecked(false);
    ui->trackAutoRadioButton->setChecked(true);
    updateTrackedParameter();
    ui->fileNameLabel->setText(QFileInfo(filePath).fileName());
    enableFilesTabButtons(true);

    QMessageBox::information(
        this,
        "Cleared Derived Channels",
        "All derived channels have been cleared successfully."
        );
}

void mainWindow::deriveButtonClicked() {
    QString ch1 = ui->ch1ComboBox->currentText();
    QString ch2 = ui->ch2ComboBox->currentText();
    QString derivedChName = ui->derivedChLineEdit->text();
    QString channelType = ui->channelTypeComboBox->currentText();

    if (ch1.isEmpty() || ch2.isEmpty() || derivedChName.isEmpty() || channelType.isEmpty()) {
        QMessageBox::warning(this, "Missing Information", "Please ensure all fields are filled out.");
        return;
    }

    // Allow letters, digits, underscore, hyphen. Disallow whitespace, colon, semicolon.
    QRegularExpression invalid(R"([^A-Za-z0-9_-])");
    if (derivedChName.contains(invalid)) {
        QMessageBox::warning(this, "Invalid Name",
                             "Use letters, digits, '_' or '-'. No spaces or : ;");
        return;
    }

    derivedChName = derivedChName.toUpper();

    // Unique name
    for (const auto &sensor : currentSpan->sensors) {
        if (QString::fromStdString(sensor.second) == derivedChName) {
            QMessageBox::warning(this, "Duplicate Name",
                                 "A channel with this name already exists.");
            return;
        }
    }

    // Inputs
    std::vector<posData> signal1 = getSignalData(ch1);
    std::vector<posData> signal2 = getSignalData(ch2);

    if (signal1.size() != signal2.size()) {
        QMessageBox::warning(this, "Signal Size Mismatch",
                             "The input channels have different sizes.");
        return;
    }

    std::vector<posData> derivedSignal;
    QString method;

    if (channelType == "Subtract a component") {
        derivedSignal = SignalProcessing::subtractComponent(signal1, signal2);
        method = "SUB";
    } else if (channelType == "Compute Euclidean distance") {
        derivedSignal = SignalProcessing::computeEuclideanDistance(signal1, signal2);
        method = "DIST";
    }

    // Store derived
    currentSpan->channelsData.push_back(derivedSignal);
    int newChannelIndex = static_cast<int>(currentSpan->channelsData.size());
    currentSpan->sensors[newChannelIndex] = derivedChName.toStdString();

    updateRecipeLine(derivedChName, ch1, ch2, method);

    // Remember parents for color mixing
    m_derivedParents[derivedChName] = qMakePair(ch1, ch2);
    m_configColorCache.remove(derivedChName); // just in case

    // Update UI lists
    ui->channelsComboBox->addItem(derivedChName);
    ui->ch1ComboBox->addItem(derivedChName);
    ui->ch2ComboBox->addItem(derivedChName);

    ui->derivedChLineEdit->clear();
    ui->derivedChLineEdit->setPlaceholderText("Type here");
    ui->channelTypeComboBox->setCurrentIndex(0);
    ui->ch1ComboBox->setCurrentIndex(0);
    ui->ch2ComboBox->setCurrentIndex(0);

    QMessageBox::information(this, "Derived Channel",
                             "Derived channel successfully created.");
}

void mainWindow::updateRecipeLine(const QString &derivedName,
                                  const QString &signal1,
                                  const QString &signal2,
                                  const QString &option)
{
    if (!currentSpan) return;

    SpanIO::updateRecipeEntry(
        currentSpan->path,
        derivedName.toStdString(),
        signal1.toStdString(),
        signal2.toStdString(),
        option.toStdString()
        );

    refreshSpanFileRow(QString::fromStdString(currentSpan->path));
}

std::vector<posData> mainWindow::getSignalData(const QString &channelName) {
    for (const auto &sensor : currentSpan->sensors) {
        if (QString::fromStdString(sensor.second) == channelName) {
            int channelIndex = sensor.first - 1;
            if (channelIndex >= 0 && channelIndex < static_cast<int>(currentSpan->channelsData.size())) {
                return currentSpan->channelsData[channelIndex];
            }
        }
    }
    return {};
}

double mainWindow::getLandmarkYValue(const QString &channelName, double offset) const
{
    for (auto *visualizer : kinematicVisualizers) {
        if (!visualizer)
            continue;

        if (visualizer->configName().trimmed() != channelName.trimmed())
            continue;

        QString usedAxis;
        return visualizer->getSignalValueAt(offset, &usedAxis);
    }

    return std::numeric_limits<double>::quiet_NaN();
}

// ------------------------ Template Processing ------------------------

void mainWindow::autoAnnotateChannelAtTime(const QString &channelName,
                                           double timeSeconds)
{
    if (timeSeconds < xAxisMinLimit)
        timeSeconds = xAxisMinLimit;
    if (timeSeconds > xAxisMaxLimit)
        timeSeconds = xAxisMaxLimit;

    for (auto *visualizer : kinematicVisualizers) {
        if (!visualizer)
            continue;

        if (visualizer->configName().trimmed() != channelName.trimmed())
            continue;

        QCustomPlot *plot = visualizer->getCustomPlot();
        if (!plot)
            continue;

        // Store the desired X (seconds) directly on the plot.
        plot->setProperty("autoAnnotateX", timeSeconds);

        const QRect ar = plot->axisRect()->rect();
        const QPoint pos(ar.center().x(), ar.center().y());

        const QPointF localPos(pos);
        const QPointF globalPos(plot->mapToGlobal(pos));

        QMouseEvent ev(QEvent::MouseButtonPress,
                       localPos,
                       globalPos,
                       Qt::RightButton,
                       Qt::RightButton,
                       Qt::ShiftModifier);

        QCoreApplication::sendEvent(plot, &ev);

        // Label's handler already clears autoAnnotateX once it uses it,
        // but this is harmless extra safety:
        plot->setProperty("autoAnnotateX", QVariant());

        break;
    }
}

void mainWindow::onSpecApplyButtonClicked()
{
    if (!currentSpan) return;
    rebuildSpectrogramForVisibleRange();
}

void mainWindow::onLandmarkDetectionApplyClicked()
{
    if (ui->landmarkThresholdSpinBox) {
        m_appliedVelocityThresholdFraction = ui->landmarkThresholdSpinBox->value();
    }

    if (ui->reopeningDeadbandSpinBox) {
        m_appliedReopeningDeadbandFraction = ui->reopeningDeadbandSpinBox->value();
    }

    applyLandmarkDetectionSettingsToAllLabels();
}

void mainWindow::onLandmarkDetectionDefaultsClicked()
{
    if (ui->landmarkThresholdSpinBox) {
        ui->landmarkThresholdSpinBox->setValue(0.20);
    }

    if (ui->reopeningDeadbandSpinBox) {
        ui->reopeningDeadbandSpinBox->setValue(0.05);
    }
}



void mainWindow::onSpecDefaultsButtonClicked()
{
    if (ui->specWindowMsSpinBox)
        ui->specWindowMsSpinBox->setValue(SignalProcessing::kSpecDefaultWindowMs);
    if (ui->specViewMaxHzSpinBox)
        ui->specViewMaxHzSpinBox->setValue(SignalProcessing::kSpecDefaultMaxFrequencyHz);
    if (ui->specDynamicRangeSpinBox)
        ui->specDynamicRangeSpinBox->setValue(SignalProcessing::kSpecDefaultDynRangeDb);

    onSpecApplyButtonClicked();
}

void mainWindow::on_saveTemplateButton_clicked()
{
    if (!currentSpan) {
        QMessageBox::warning(this, tr("No file loaded"),
                             tr("Load a SPAN file first before saving a template."));
        return;
    }

    // Directory = folder of the current .span file
    const QString spanPath = QString::fromStdString(currentSpan->path);
    const QFileInfo fi(spanPath);
    const QString templateDir = fi.absolutePath();
    m_templateManager->setTemplateDirectory(templateDir);

    const QString templateId = QInputDialog::getText(
                                   this,
                                   tr("Save template"),
                                   tr("Template ID (e.g. sentence/nonword code):")
                                   ).trimmed();

    if (templateId.isEmpty())
        return;

    // Collect MaxC landmarks per channel from the landmark table
    QMap<QString, QVector<double>> tMaxCPerChannel;

    for (int row = 0; row < landmarkListModel->rowCount(); ++row) {
        const QString name   = landmarkListModel->item(row, 0)->text();
        const QString chan   = landmarkListModel->item(row, 1)->text();
        const QString offStr = landmarkListModel->item(row, 2)->text();

        bool ok = false;
        const double t = offStr.toDouble(&ok);
        if (!ok) continue;

        if (name.compare("maxc", Qt::CaseInsensitive) != 0)
            continue;

        tMaxCPerChannel[chan].append(t);
    }

    if (tMaxCPerChannel.isEmpty()) {
        QMessageBox::warning(this, tr("No MaxC landmarks"),
                             tr("Place at least one MaxC landmark on the exemplar file "
                                "before saving a template."));
        return;
    }

    GestureTemplate tpl;
    tpl.id = templateId;

    // SR used for time axis / labels
    double timeSR = (currentSpan->wavSR > 0)
                        ? double(currentSpan->wavSR)
                        : double(currentSpan->posSR);
    tpl.sampleRate = timeSR;

    for (auto it = tMaxCPerChannel.constBegin();
         it != tMaxCPerChannel.constEnd(); ++it)
    {
        const QString         &chanName = it.key();
        const QVector<double> &times    = it.value();

        // Find config / samples for this channel
        const SensorConfig *cfg = findConfigForChannel(*currentSpan, chanName);
        if (!cfg)
            continue;

        QVector<double> sigScalar;
        if (!extractScalarSeriesForConfig(*currentSpan, *cfg, sigScalar))
            continue;

        GestureTemplateChannelData cd;
        cd.samples  = sigScalar;
        cd.cfgX     = cfg->x;
        cd.cfgY     = cfg->y;
        cd.cfgZ     = cfg->z;
        cd.cfgParam = motionParamToInt(cfg->param);

        cd.idxMaxCList.clear();

        const double sr = timeSR;

        for (double tMaxC : times) {
            int idx = 0;
            if (sr > 0.0 && !cd.samples.isEmpty()) {
                idx = int(std::lround(tMaxC * sr));
                if (idx < 0)
                    idx = 0;
                if (idx >= cd.samples.size())
                    idx = cd.samples.size() - 1;
            }
            cd.idxMaxCList.append(idx);
        }

        // convenience single index (first MaxC) if you still use it anywhere
        cd.idxMaxC = cd.idxMaxCList.isEmpty() ? -1 : cd.idxMaxCList.first();

        tpl.channels.insert(chanName, cd);
    }

    if (tpl.channels.isEmpty()) {
        QMessageBox::warning(this, tr("Template error"),
                             tr("No usable channels found to save in the template."));
        return;
    }

    if (!m_templateManager->saveTemplate(tpl)) {
        QMessageBox::warning(this, tr("Template error"),
                             tr("Failed to save template \"%1\".").arg(templateId));
        return;
    }

    ui->templateComboBox->clear();
    ui->templateComboBox->addItems(m_templateManager->availableTemplateIds());
    ui->templateComboBox->setCurrentText(templateId);

    QMessageBox::information(this, tr("Template saved"),
                             tr("Template \"%1\" saved next to \"%2\".")
                                 .arg(templateId, fi.fileName()));
}

void mainWindow::on_applyTemplateButton_clicked()
{
    if (!currentSpan) {
        QMessageBox::warning(this,
                             tr("No file loaded"),
                             tr("Load a SPAN file first before applying a template."));
        return;
    }

    // Template directory = folder of current .span file
    const QString spanPath = QString::fromStdString(currentSpan->path);
    const QFileInfo fi(spanPath);

    const QString templateDir = fi.absolutePath();
    if (m_templateManager)
        m_templateManager->setTemplateDirectory(templateDir);

    const QString tplId = ui->templateComboBox->currentText().trimmed();
    if (tplId.isEmpty()) {
        QMessageBox::warning(this,
                             tr("No template selected"),
                             tr("Select a template in the combo box first."));
        return;
    }

    // DTW parameters from UI
    const int uiRateHz = ui->dtwRateSpinBox
                             ? ui->dtwRateSpinBox->value()
                             : 200;
    const int uiMaxLen = ui->dtwMaxLenSpinBox
                             ? ui->dtwMaxLenSpinBox->value()
                             : 5000;

    if (!m_templateManager) {
        QMessageBox::warning(this,
                             tr("Template manager"),
                             tr("Template manager is not initialized."));
        return;
    }

    GestureTemplate tpl;
    if (!m_templateManager->loadTemplate(tplId, tpl)) {
        QMessageBox::warning(this,
                             tr("Template error"),
                             tr("Could not load template \"%1\".").arg(tplId));
        return;
    }

    // let manager do DTW + alignment
    auto res = m_templateManager->applyTemplateToSpan(*currentSpan, tpl,
                                                      uiRateHz, uiMaxLen);
    if (!res.error.isEmpty()) {
        QMessageBox::warning(this, tr("Template error"), res.error);
        return;
    }

    // Turn the times into actual labels via your existing helper
    for (auto it = res.timesByChannel.constBegin();
         it != res.timesByChannel.constEnd(); ++it)
    {
        const QString        &chanName = it.key();
        const QVector<double> times    = it.value();

        for (double t : times) {
            // Let Label decide locally whether this is nearer a max or min.
            autoAnnotateChannelAtTime(chanName, t);
        }
    }

    // Let Label finish emitting landmarkAdded signals
    int lastCount = -1;
    for (int iter = 0; iter < 5; ++iter) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
        int currentCount = landmarkListModel->rowCount();
        if (currentCount == lastCount)
            break;
        lastCount = currentCount;
    }

    QMessageBox::information(
        this,
        tr("Template applied"),
        tr("Template \"%1\" applied to %2 channel(s) in this file.\n"
           "Use \"Save CSV\" if you want to export the landmarks.")
            .arg(tplId)
            .arg(res.appliedChannels));
}


void mainWindow::importTemplateFromFile()
{
    if (!currentSpan) {
        QMessageBox::warning(this, tr("No file loaded"),
                             tr("Load a SPAN file first before importing a template."));
        return;
    }

    // Let the user pick an existing JSON template
    const QString srcPath = QFileDialog::getOpenFileName(
        this,
        tr("Import Gesture Template"),
        QString(),
        tr("Gesture Templates (*.json)")
        );
    if (srcPath.isEmpty())
        return;

    QFileInfo srcInfo(srcPath);
    if (!srcInfo.exists()) {
        QMessageBox::warning(this, tr("File not found"),
                             tr("The selected file does not exist."));
        return;
    }

    // Target: directory of the currently loaded SPAN file
    const QString spanPath = QString::fromStdString(currentSpan->path);
    QFileInfo spanInfo(spanPath);
    const QString templateDir = spanInfo.absolutePath();

    QDir dir(templateDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            QMessageBox::warning(
                this,
                tr("Directory error"),
                tr("Could not access template directory:\n%1").arg(templateDir)
                );
            return;
        }
    }

    QString baseName = srcInfo.completeBaseName();
    QString dstPath  = dir.absoluteFilePath(baseName + ".json");

    // Avoid silently overwriting an existing template: append suffix if needed
    if (QFile::exists(dstPath)) {
        int counter = 1;
        while (QFile::exists(dstPath)) {
            dstPath = dir.absoluteFilePath(
                QString("%1_copy%2.json").arg(baseName).arg(counter++)
                );
        }
    }

    if (!QFile::copy(srcPath, dstPath)) {
        QMessageBox::warning(
            this,
            tr("Copy failed"),
            tr("Could not copy template to:\n%1").arg(dstPath)
            );
        return;
    }

    // Keep manager + combo box in sync
    if (m_templateManager) {
        m_templateManager->setTemplateDirectory(templateDir);
    }

    reloadTemplatesForCurrentDir();

    QMessageBox::information(
        this,
        tr("Template imported"),
        tr("Template \"%1\" has been imported.")
            .arg(QFileInfo(dstPath).completeBaseName())
        );
}

void mainWindow::on_batchApplyTemplateButton_clicked()
{
    // 1) Basic checks
    if (!currentSpan) {
        QMessageBox::warning(this, tr("No file loaded"),
                             tr("Load an exemplar SPAN file first."));
        return;
    }

    const QString tplId = ui->templateComboBox->currentText().trimmed();
    if (tplId.isEmpty()) {
        QMessageBox::warning(this, tr("No template selected"),
                             tr("Select a template in the combo box first."));
        return;
    }

    // DTW params from UI
    const int uiRateHz = ui->dtwRateSpinBox
                             ? ui->dtwRateSpinBox->value()
                             : 200;
    const int uiMaxLen = ui->dtwMaxLenSpinBox
                             ? ui->dtwMaxLenSpinBox->value()
                             : 5000;

    // Directory of exemplar .span (and templates)
    const QString exemplarSpanPath = QString::fromStdString(currentSpan->path);
    const QFileInfo exemplarInfo(exemplarSpanPath);
    const QString templateDir = exemplarInfo.absolutePath();

    if (!m_templateManager) {
        QMessageBox::warning(this,
                             tr("Template manager"),
                             tr("Template manager is not initialized."));
        return;
    }
    m_templateManager->setTemplateDirectory(templateDir);

    // Load the chosen template ONCE
    GestureTemplate tpl;
    if (!m_templateManager->loadTemplate(tplId, tpl)) {
        QMessageBox::warning(this,
                             tr("Template error"),
                             tr("Could not load template \"%1\" from\n%2")
                                 .arg(tplId, templateDir));
        return;
    }

    // 2) Build list of target .span files in the SAME folder, from your list
    QStringList targetFiles;

    // Prefer: selected rows in filesListView
    auto *selModel = ui->filesListView->selectionModel();
    if (selModel && selModel->hasSelection()) {
        const QModelIndexList rows = selModel->selectedRows(); // column 0
        for (const QModelIndex &idx : rows) {
            const QString fullPath =
                idx.sibling(idx.row(), 0).data(Qt::UserRole).toString();
            QFileInfo fi(fullPath);
            if (fi.suffix().compare("span", Qt::CaseInsensitive) != 0)
                continue;
            if (fi.absolutePath() != templateDir)
                continue;
            targetFiles << fi.absoluteFilePath();
        }
    } else {
        // Fall back: all span files in the same folder as exemplar
        for (const QString &fn : spanFilesNames) {
            QFileInfo fi(fn);
            if (fi.suffix().compare("span", Qt::CaseInsensitive) != 0)
                continue;
            if (fi.absolutePath() != templateDir)
                continue;
            targetFiles << fi.absoluteFilePath();
        }
    }

    if (targetFiles.isEmpty()) {
        QMessageBox::information(
            this,
            tr("No files to annotate"),
            tr("There are no .span files in the same folder as the exemplar:\n%1")
                .arg(templateDir));
        return;
    }

    // Confirmation
    if (QMessageBox::question(
            this,
            tr("Batch apply template"),
            tr("Apply template \"%1\" with DTW to %2 file(s) in:\n%3\n\n"
               "Landmarks will be generated and saved as CSV next to each file.")
                .arg(tplId)
                .arg(targetFiles.size())
                .arg(templateDir))
        != QMessageBox::Yes)
    {
        return;
    }

    // Remember which file was open before batch
    QString originalSpanPath = exemplarSpanPath;

    int total   = targetFiles.size();
    int success = 0;
    QStringList failed;

    // -------------------- PROGRESS DIALOG SETUP --------------------
    QProgressDialog progress(
        tr("Applying template \"%1\" ...").arg(tplId),
        tr("Cancel"),
        0,
        total,
        this
        );
    progress.setWindowModality(Qt::ApplicationModal);
    progress.setMinimumDuration(0);     // show immediately
    progress.setValue(0);

    bool cancelled = false;
    // ---------------------------------------------------------------

    // 3) Main loop: one file at a time
    for (int i = 0; i < targetFiles.size(); ++i) {
        const QString filePath = targetFiles.at(i);
        const QFileInfo fi(filePath);

        // Update progress UI for this file
        progress.setValue(i);  // i files already processed
        progress.setLabelText(
            tr("Applying template \"%1\" (%2/%3):\n%4")
                .arg(tplId)
                .arg(i + 1)
                .arg(total)
                .arg(fi.fileName())
            );

        // Let the UI breathe and show progress in the window title
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

        // User hit "Cancel"?
        if (progress.wasCanceled()) {
            cancelled = true;
            failed << filePath + tr(" (cancelled by user)");
            break;
        }

        setWindowTitle(tr("SPAN – Batch DTW %1/%2: %3")
                           .arg(i + 1)
                           .arg(total)
                           .arg(fi.fileName()));

        // Load the span into UI (plots, labels, 3D etc.)
        if (!loadSpanIntoUi(filePath)) {
            failed << filePath + tr(" (failed to load)");
            continue;
        }

        // Just to be explicit – loadSpanIntoUi already cleared plots/landmarks.
        clearAllLandmarks();

        // Run DTW against this file
        auto res = m_templateManager->applyTemplateToSpan(*currentSpan,
                                                          tpl,
                                                          uiRateHz,
                                                          uiMaxLen);
        if (!res.ok()) {
            const QString reason =
                res.error.isEmpty()
                    ? tr("no channels matched the template")
                    : res.error;
            failed << filePath + " (" + reason + ")";
            continue;
        }

        // Use the existing UI-based pipeline to create landmarks:
        // this will internally fire Label logic via fake right-clicks.
        for (auto it = res.timesByChannel.constBegin();
             it != res.timesByChannel.constEnd(); ++it)
        {
            const QString        &chanName = it.key();
            const QVector<double> times    = it.value();

            for (double t : times) {
                autoAnnotateChannelAtTime(chanName, t);
            }
        }

        // Let all landmarkAdded signals land in the model
        int lastCount = -1;
        for (int iter = 0; iter < 5; ++iter) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
            int currentCount = landmarkListModel->rowCount();
            if (currentCount == lastCount)
                break;
            lastCount = currentCount;
        }

        if (landmarkListModel->rowCount() == 0) {
            failed << filePath + tr(" (template applied but no landmarks created)");
            continue;
        }

        // Decide CSV output path: <base>.csv next to the .span
        const QString baseName = fi.completeBaseName();
        const QString csvPath  = fi.absolutePath() + "/"
                                + baseName + ".csv";

        QString errorMsg;
        if (!writeCurrentLandmarksToCsv(csvPath, &errorMsg, /*showSuccessMessage=*/false)) {
            failed << filePath + " (" + errorMsg + ")";
            continue;
        }

        ++success;

        // Advance progress after successfully finishing this file
        progress.setValue(i + 1);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
    }

    // Make sure the progress bar hits the end if we didn't cancel early
    if (!cancelled)
        progress.setValue(total);

    // 4) Restore the original exemplar if it still exists
    if (!originalSpanPath.isEmpty() && QFile::exists(originalSpanPath)) {
        loadSpanIntoUi(originalSpanPath);
    }

    setWindowTitle(tr("SPAN"));

    // 5) Summary for the user
    const int failedCount = failed.size();
    QString msg = tr("Batch DTW finished.\n\n"
                     "Template: %1\n"
                     "Folder:   %2\n"
                     "Total files:         %3\n"
                     "Successfully annotated: %4\n"
                     "Failed:              %5")
                      .arg(tplId)
                      .arg(templateDir)
                      .arg(total)
                      .arg(success)
                      .arg(failedCount);

    if (cancelled) {
        msg.prepend(tr("Operation was cancelled by the user.\n\n"));
    }

    if (!failed.isEmpty()) {
        msg += "\n\n" + tr("Failures:\n") + failed.join("\n");
    }

    QMessageBox::information(this, tr("Batch template application"), msg);
}


// ------------------------ Landmarks & CSV ------------------------

bool mainWindow::writeCurrentLandmarksToCsv(const QString &csvFilePath,
                                            QString *errorMessage,
                                            bool showSuccessMessage)
{
    if (!currentSpan) {
        if (errorMessage)
            *errorMessage = tr("No SPAN file is loaded.");
        return false;
    }

    if (landmarkListModel->rowCount() == 0) {
        if (errorMessage)
            *errorMessage = tr("There are no landmarks to save.");
        return false;
    }

    QFile csvFile(csvFilePath);
    if (!csvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorMessage) {
            *errorMessage = tr("Cannot open file for writing:\n%1")
            .arg(csvFilePath);
        }
        return false;
    }

    QTextStream out(&csvFile);

    // Unified CSV format for manual + auto + batch
    out << "Name,Channel,Offset,Y\n";
    for (int row = 0; row < landmarkListModel->rowCount(); ++row) {
        const QString name      = landmarkListModel->item(row, 0)->text();
        const QString channel   = landmarkListModel->item(row, 1)->text();
        const QString offsetStr = landmarkListModel->item(row, 2)->text();
        const QString yStr      = (landmarkListModel->columnCount() > 3 && landmarkListModel->item(row, 3))
                                 ? landmarkListModel->item(row, 3)->text()
                                 : QString();

        bool okOffset = false;
        const double offsetVal = offsetStr.toDouble(&okOffset);
        if (!okOffset || std::isnan(offsetVal)) {
            continue;
        }

        bool okY = false;
        const double yVal = yStr.toDouble(&okY);

        out << name << ","
            << channel << ","
            << QString::number(offsetVal, 'f', 3) << ",";

        if (okY && std::isfinite(yVal))
            out << QString::number(yVal, 'f', 3);

        out << "\n";
    }

    csvFile.close();

    if (showSuccessMessage) {
        QMessageBox::information(
            this,
            tr("CSV Saved"),
            tr("Landmarks have been saved to:\n%1").arg(csvFilePath)
            );
    }

    return true;
}


void mainWindow::getCSVFileButtonClicked()
{
    if (!currentSpan) {
        QMessageBox::warning(this,
                             tr("No SPAN File Loaded"),
                             tr("Please load a SPAN file first."));
        return;
    }

    if (landmarkListModel->rowCount() == 0) {
        QMessageBox::information(this,
                                 tr("No Landmarks"),
                                 tr("There are no landmarks to save."));
        return;
    }

    const QString spanPath = QString::fromStdString(currentSpan->path);
    const QFileInfo spanFileInfo(spanPath);
    const QString defaultName = spanFileInfo.completeBaseName() + ".csv";

    const QString initialDir =
        lastCsvExportDir.isEmpty()
            ? spanFileInfo.absolutePath()
            : lastCsvExportDir;

    const QString initialPath = QDir(initialDir).filePath(defaultName);

    const QString csvFilePath = QFileDialog::getSaveFileName(
        this,
        tr("Save CSV File"),
        initialPath,
        tr("CSV Files (*.csv)")
        );

    if (csvFilePath.isEmpty())
        return;

    QString error;
    if (!writeCurrentLandmarksToCsv(csvFilePath, &error, true)) {
        QMessageBox::warning(this, tr("CSV Error"), error);
        return;
    }

    lastCsvExportDir = QFileInfo(csvFilePath).absolutePath();
}


void mainWindow::clearAllLandmarks() {
    if (landmarkListModel->rowCount() == 0) {
        enableConfigureTabButtons(false);
        return;
    }

    globalNameCounter = 1;
    currentLandmarkCount = 1;

    landmarkListModel->removeRows(0, landmarkListModel->rowCount());

    for (auto visualizer : kinematicVisualizers) {
        if (auto plot = visualizer->getCustomPlot()) {
            if (auto label = visualizer->getLabel()) {
                label->removeAllLabels();
            }
        }
    }
    enableConfigureTabButtons(false);
}

void mainWindow::removeSelectedLandmark()
{
    QModelIndex idx = ui->landmarkListView->currentIndex();
    if (!idx.isValid())
        return;

    const QString chan = idx.siblingAtColumn(1).data().toString();
    const double  x    = idx.siblingAtColumn(2).data().toDouble();

    // Remove only from the matching plot/channel
    for (auto v : kinematicVisualizers) {
        if (!v)
            continue;

        if (v->configName().trimmed() != chan.trimmed())
            continue;

        if (auto lbl = v->getLabel()) {
            lbl->removeLabelAt(x, chan);
            lbl->clearSelectedLine();
        }

        if (auto plot = v->getCustomPlot())
            plot->replot();

        break; // only one matching visualizer should handle this landmark
    }

    // Remove the selected row from the model
    landmarkListModel->removeRow(idx.row());
    ui->landmarkListView->clearSelection();

    if (landmarkListModel->rowCount() == 0)
        enableConfigureTabButtons(false);
}

void mainWindow::removeLandmarkFromModel(double x) {
    for (int row = 0; row < landmarkListModel->rowCount(); ++row) {
        QModelIndex idx = landmarkListModel->index(row, 2);
        if (qAbs(idx.data().toDouble() - x) < 1e-6) {
            landmarkListModel->removeRow(row);
            break;
        }
    }
}

// ------------------------ Zoom / selection ------------------------

void mainWindow::zoomInButtonClicked()
{
    if (kinematicVisualizers.isEmpty())
        return;

    QCustomPlot *plot = kinematicVisualizers.first()->getCustomPlot();
    if (!plot)
        return;

    const double zoomFactor = 0.98; // in

    QCPRange currentRange = plot->xAxis->range();
    double rangeSize = currentRange.size();
    double center    = currentRange.center();

    double halfRange = (rangeSize * zoomFactor) / 2.0;
    double lower = std::max(center - halfRange, xAxisMinLimit);
    double upper = std::min(center + halfRange, xAxisMaxLimit);

    plot->xAxis->setRange(lower, upper);
    plot->xAxis2->setRange(lower, upper);
    plot->replot(QCustomPlot::rpQueuedReplot);

    updateScrollBar();
    rebuildSpectrogramForVisibleRange();
}

void mainWindow::zoomOutButtonClicked()
{
    if (kinematicVisualizers.isEmpty())
        return;

    QCustomPlot *plot = kinematicVisualizers.first()->getCustomPlot();
    if (!plot)
        return;

    const double zoomFactor = 1.02; // out

    QCPRange currentRange = plot->xAxis->range();
    double rangeSize = currentRange.size();
    double center    = currentRange.center();

    double halfRange = (rangeSize * zoomFactor) / 2.0;
    double lower = std::max(center - halfRange, xAxisMinLimit);
    double upper = std::min(center + halfRange, xAxisMaxLimit);

    plot->xAxis->setRange(lower, upper);
    plot->xAxis2->setRange(lower, upper);
    plot->replot(QCustomPlot::rpQueuedReplot);

    updateScrollBar();
    rebuildSpectrogramForVisibleRange();
}

void mainWindow::resetButtonClicked()
{
    if (kinematicVisualizers.isEmpty())
        return;

    QCustomPlot *plot = kinematicVisualizers.first()->getCustomPlot();
    if (!plot)
        return;

    plot->xAxis->setRange(xAxisMinLimit, xAxisMaxLimit);
    plot->xAxis2->setRange(xAxisMinLimit, xAxisMaxLimit);
    plot->replot(QCustomPlot::rpQueuedReplot);

    updateScrollBar();
    rebuildSpectrogramForVisibleRange();
}


void mainWindow::on_selectButton_clicked()
{
    double selectionStart = std::numeric_limits<double>::max();
    double selectionEnd   = std::numeric_limits<double>::lowest();
    bool hasSelection = false;

    for (auto visualizer : kinematicVisualizers) {
        QCPRange range = visualizer->getSelectionRange();
        if (range.lower < range.upper) {
            selectionStart = std::min(selectionStart, range.lower);
            selectionEnd   = std::max(selectionEnd,   range.upper);
            hasSelection = true;
        }
    }

    if (!hasSelection)
        return;

    if (kinematicVisualizers.isEmpty())
        return;

    QCustomPlot *plot = kinematicVisualizers.first()->getCustomPlot();
    if (!plot)
        return;

    double newCenter = (selectionStart + selectionEnd) / 2.0;
    double newRange  = selectionEnd - selectionStart;

    double newLower = std::max(xAxisMinLimit, newCenter - newRange / 2.0);
    double newUpper = std::min(xAxisMaxLimit, newCenter + newRange / 2.0);

    plot->xAxis->setRange(newLower, newUpper);
    plot->xAxis2->setRange(newLower, newUpper);
    plot->replot(QCustomPlot::rpQueuedReplot);

    updateScrollBar();
    rebuildSpectrogramForVisibleRange();
}

void mainWindow::updateTrackedParameter() {
    QString parameter;
    if (ui->trackXRadioButton->isChecked())      parameter = "X";
    else if (ui->trackYRadioButton->isChecked()) parameter = "Y";
    else if (ui->trackZRadioButton->isChecked()) parameter = "Z";

    for (auto visualizer : kinematicVisualizers)
        visualizer->setTrackedParameter(parameter);
}


// ------------------------ Visualization ------------------------

void mainWindow::rebuildSpectrogramForVisibleRange()
{
    if (m_updatingSpectrogram)
        return;
    if (!currentSpan)
        return;
    if (kinematicVisualizers.isEmpty())
        return;

    // Find the spectrogram visualizer
    KinematicVisualizer *specVis = nullptr;
    for (auto *v : kinematicVisualizers) {
        if (!v) continue;
        if (v->configName().compare("spectrogram", Qt::CaseInsensitive) == 0) {
            specVis = v;
            break;
        }
    }
    if (!specVis)
        return;

    QCustomPlot *refPlot = kinematicVisualizers.first()->getCustomPlot();
    if (!refPlot)
        return;

    if (currentSpan->audioData.empty() || currentSpan->wavSR <= 0)
        return;

    const int sr = currentSpan->wavSR;
    const double fullDuration =
        static_cast<double>(currentSpan->audioData.size()) / sr;

    QCPRange range = refPlot->xAxis->range();

    double tStart = std::max(0.0, range.lower);
    double tEnd   = std::min(fullDuration, range.upper);

    // If the range is too tiny or degenerate, just adjust color scale
    if (!(tEnd > tStart + 1e-3)) {
        updateSpectrogramColorScale();
        return;
    }

    const double eps = 1e-3;
    const bool fullView =
        (tStart <= eps) && (tEnd >= fullDuration - eps);

    m_updatingSpectrogram = true;

    if (fullView) {
        // Use full-file spectrogram (with cache)
        buildSpectrogramForVisualizer(specVis, *currentSpan, 0.0, -1.0);
    } else {
        // Recompute on visible time range only
        buildSpectrogramForVisualizer(specVis, *currentSpan, tStart, tEnd);
    }

    m_updatingSpectrogram = false;

    // And rescale colors for the current visible range
    updateSpectrogramColorScale();
}

void mainWindow::updateSpectrogramColorScale()
{
    if (kinematicVisualizers.isEmpty())
        return;

    // All plots share the same X range; use the first as reference
    QCustomPlot *refPlot = kinematicVisualizers.first()->getCustomPlot();
    if (!refPlot)
        return;

    const QCPRange range = refPlot->xAxis->range();

    for (auto *visualizer : kinematicVisualizers) {
        // We named the spectrogram visualizer "spectrogram" above
        if (visualizer->configName().compare("spectrogram", Qt::CaseInsensitive) == 0) {
            visualizer->updateSpectrogramColorScaleForRange(range);
            break;
        }
    }
}

void mainWindow::buildSpectrogramForVisualizer(KinematicVisualizer *visualizer,
                                               const spanFile &data,
                                               double tStartSeconds,
                                               double tEndSeconds)
{
    if (!visualizer) return;
    if (data.audioData.empty() || data.wavSR <= 0) return;

    // --- Read UI parameters (or defaults) ---
    double windowMs = ui->specWindowMsSpinBox
                          ? ui->specWindowMsSpinBox->value()
                          : SignalProcessing::kSpecDefaultWindowMs;

    double overlap  = SignalProcessing::kSpecDefaultOverlapRatio;

    int maxFreqHz   = ui->specViewMaxHzSpinBox
                        ? ui->specViewMaxHzSpinBox->value()
                        : SignalProcessing::kSpecDefaultMaxFrequencyHz;

    double dynRangeDb = ui->specDynamicRangeSpinBox
                            ? ui->specDynamicRangeSpinBox->value()
                            : SignalProcessing::kSpecDefaultDynRangeDb;

    // Clamp sanity
    if (windowMs <= 1.0)   windowMs   = 1.0;
    if (maxFreqHz <= 0)    maxFreqHz  = 1000;
    if (dynRangeDb <= 1.0) dynRangeDb = 1.0;

    const int    sr         = data.wavSR;
    const qint64 numSamples = static_cast<qint64>(data.audioData.size());
    const double fullDuration =
        static_cast<double>(numSamples) / sr;

    // --- Clamp / normalize requested time interval ---
    if (tStartSeconds < 0.0)
        tStartSeconds = 0.0;

    if (tEndSeconds <= 0.0 || tEndSeconds > fullDuration)
        tEndSeconds = fullDuration;

    if (tEndSeconds <= tStartSeconds + 1e-6) {
        // Degenerate range → fall back to full file
        tStartSeconds = 0.0;
        tEndSeconds   = fullDuration;
    }

    const bool fullFile =
        (std::abs(tStartSeconds) < 1e-6) &&
        (std::abs(tEndSeconds - fullDuration) < 1e-6);

    QVector<QVector<double>> spectrogramToUse;

    if (fullFile) {
        // ---------- USE / UPDATE CACHE FOR FULL FILE ----------
        bool needRecompute = true;

        if (m_specCacheValid) {
            if (m_specCacheSampleRate == sr &&
                m_specCacheNumSamples  == numSamples &&
                m_specCacheMaxFreqHz   == maxFreqHz &&
                qFuzzyCompare(m_specCacheWindowMs, windowMs) &&
                qFuzzyCompare(m_specCacheOverlap,  overlap))
            {
                needRecompute = false;
            }
        }

        if (needRecompute) {
            // Heavy part: FFT-based spectrogram
            auto specStd = SignalProcessing::computeSpectrogram(
                data.audioData,
                sr,
                maxFreqHz,
                windowMs,
                overlap
                );

            m_specCacheData.clear();
            m_specCacheData.reserve(static_cast<int>(specStd.size()));
            for (const auto &row : specStd) {
                QVector<double> qRow(static_cast<int>(row.size()));
                for (int i = 0; i < qRow.size(); ++i)
                    qRow[i] = row[static_cast<size_t>(i)];
                m_specCacheData.push_back(std::move(qRow));
            }

            m_specCacheValid      = true;
            m_specCacheWindowMs   = windowMs;
            m_specCacheMaxFreqHz  = maxFreqHz;
            m_specCacheOverlap    = overlap;
            m_specCacheSampleRate = sr;
            m_specCacheNumSamples = numSamples;
        }

        spectrogramToUse = m_specCacheData;
    } else {
        // ---------- RECOMPUTE ONLY ON VISIBLE TIME RANGE ----------
        const qint64 startSample =
            static_cast<qint64>(std::floor(tStartSeconds * sr));
        const qint64 endSampleExcl =
            static_cast<qint64>(std::ceil(tEndSeconds * sr));

        const qint64 clampedStart =
            std::max<qint64>(0, std::min<qint64>(startSample, numSamples - 1));
        const qint64 clampedEnd =
            std::max<qint64>(clampedStart + 1,
                             std::min<qint64>(endSampleExcl, numSamples));

        std::vector<float> segment;
        segment.reserve(static_cast<size_t>(clampedEnd - clampedStart));
        for (qint64 i = clampedStart; i < clampedEnd; ++i)
            segment.push_back(data.audioData[static_cast<size_t>(i)]);

        auto specStd = SignalProcessing::computeSpectrogram(
            segment,
            sr,
            maxFreqHz,
            windowMs,
            overlap
            );

        spectrogramToUse.clear();
        spectrogramToUse.reserve(static_cast<int>(specStd.size()));
        for (const auto &row : specStd) {
            QVector<double> qRow(static_cast<int>(row.size()));
            for (int i = 0; i < qRow.size(); ++i)
                qRow[i] = row[static_cast<size_t>(i)];
            spectrogramToUse.push_back(std::move(qRow));
        }
    }

    // Draw (or redraw); here tStart/tEnd define the *absolute* time range
    visualizer->visualizeSpectrogram(
        spectrogramToUse,
        "Spectrogram",
        tStartSeconds,
        tEndSeconds,
        maxFreqHz,
        dynRangeDb
        );
}


void mainWindow::visualizeSignal(spanFile &data) {
    // Call clearAllPlots() BEFORE this when rebuilding.

    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    const int fixedHeight = 150;

    ui->posPlotVL->setSpacing(0);
    ui->audioPlotVL->setContentsMargins(0, 0, 0, 0);
    ui->audioPlotVL->setSpacing(0);

    ui->posPlotVL->setContentsMargins(0, 0, 0, 0);
    ui->posPlotVL->setSpacing(0);

    // ---- Audio waveform ----
    if (!data.audioData.empty()) {
        KinematicVisualizer *audioVisualizer = new KinematicVisualizer(this);
        audioVisualizer->setConfigName("audio");
        audioVisualizer->setMinimumHeight(fixedHeight);

        audioVisualizer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        audioVisualizer->setFixedHeight(150);

        QVector<double> audioDataVector(data.audioData.begin(), data.audioData.end());
        QMap<QString, QVector<double>> audioDataMap;
        audioDataMap["Audio"] = audioDataVector;

        audioVisualizer->visualizeSignal(audioDataMap, "Audio", 1.5, data.wavSR);

        // update 3D when moving the mouse over the audio plot
        if (QCustomPlot *plot = audioVisualizer->getCustomPlot()) {
            plot->setMouseTracking(true); // <-- important: get mouseMove even without button pressed

            connect(plot, &QCustomPlot::mouseMove, this,
                    [this, plot](QMouseEvent *event) {
                        if (!currentSpan) return;

                        double t = plot->xAxis->pixelToCoord(event->pos().x());
                        if (t < 0.0) return;

                        // clamp to file duration so we don't go out of range
                        const double duration = currentSpan->audioData.empty()
                                                    ? 0.0
                                                    : double(currentSpan->audioData.size() - 1) / currentSpan->wavSR;

                        if (duration <= 0.0) return;
                        if (t > duration) t = duration;

                        updateSensor3DAt(t);
                    });
        }

        ui->audioPlotVL->addWidget(audioVisualizer);
        kinematicVisualizers.append(audioVisualizer);

        // Spectrogram
        KinematicVisualizer *spectrogramVisualizer = new KinematicVisualizer(this);
        spectrogramVisualizer->setSizePolicy(sizePolicy);
        spectrogramVisualizer->setMinimumHeight(fixedHeight);
        spectrogramVisualizer->setConfigName("spectrogram");
        spectrogramVisualizer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        spectrogramVisualizer->setFixedHeight(100);

        buildSpectrogramForVisualizer(spectrogramVisualizer, data);

        ui->audioPlotVL->addWidget(spectrogramVisualizer);
        kinematicVisualizers.append(spectrogramVisualizer);

        if (Label *label = audioVisualizer->getLabel()) {
            connect(label, &Label::landmarkAdded, this, &mainWindow::onLandmarkAddedFromLabel);
            connect(label, &Label::labelClicked,  this, &mainWindow::onLabelClicked);
            connect(label, &Label::labelMoved,    this, &mainWindow::onLabelMoved, Qt::DirectConnection);
            m_allLabels.push_back(label);

            label->setVelocityThresholdFraction(m_appliedVelocityThresholdFraction);
            label->setReopeningDeadbandFraction(m_appliedReopeningDeadbandFraction);
        }

        QTimer::singleShot(0, this, [this]{ syncAudioWidthToKinematicViewport(); });
    }

    // ---------- assign stable palette indices to kinematic channels ----------

    for (const auto &config : data.configurations) {
        // Find base sensor name (strip i/v/a/x/y/z)
        const QString baseConfigName = baseSensorName(config.name);

        auto it = std::find_if(
            data.sensors.begin(), data.sensors.end(),
            [&baseConfigName, &config](const std::pair<int, std::string> &sensor) {
                const QString s = QString::fromStdString(sensor.second).trimmed();
                return s == baseConfigName.trimmed() || s == config.name.trimmed();
            });

        int channelIndex = -1;
        if (it != data.sensors.end()) channelIndex = it->first - 1;

        if (channelIndex < 0
            || channelIndex >= static_cast<int>(data.channelsData.size())
            || data.channelsData[channelIndex].empty())
            continue;

        const int dimCount = (config.x ? 1 : 0) + (config.y ? 1 : 0) + (config.z ? 1 : 0);

        bool plotScalar = false;
        QVector<double> scalarSeries;
        std::vector<posData> dataToVisualize;

        switch (config.param) {
        case motionParam::Velocity: {
            if (dimCount <= 1) {
                // per-component velocity
                dataToVisualize = SignalProcessing::calculateVelocity(
                    data.channelsData[channelIndex], data.wavSR);
            } else {
                // tangential speed |v|
                const auto v = SignalProcessing::calculateVelocity(data.channelsData[channelIndex],
                                                                   data.wavSR);
                scalarSeries.resize(static_cast<int>(v.size()));
                for (int i = 0; i < scalarSeries.size(); ++i) {
                    const double vx = v[size_t(i)].x, vy = v[size_t(i)].y, vz = v[size_t(i)].z;
                    scalarSeries[i] = std::sqrt(vx*vx + vy*vy + vz*vz);
                }
                plotScalar = true;
            }
            break;
        }
        case motionParam::Acceleration: {
            if (dimCount <= 1) {
                const auto v = SignalProcessing::calculateVelocity(
                    data.channelsData[channelIndex], data.wavSR);
                dataToVisualize = SignalProcessing::calculateAcceleration(v, data.wavSR);
            } else {
                // tangential acceleration a_t = (a·v)/|v|
                const auto v  = SignalProcessing::calculateVelocity(
                    data.channelsData[channelIndex], data.wavSR);
                const auto aa = SignalProcessing::calculateAcceleration(v, data.wavSR);
                const double eps = 1e-12;
                const int N = static_cast<int>(std::min(v.size(), aa.size()));
                scalarSeries.resize(N);
                for (int i = 0; i < N; ++i) {
                    const double vx = v[size_t(i)].x,  vy = v[size_t(i)].y,  vz = v[size_t(i)].z;
                    const double ax = aa[size_t(i)].x, ay = aa[size_t(i)].y, az = aa[size_t(i)].z;
                    const double speed = std::sqrt(vx*vx + vy*vy + vz*vz);
                    scalarSeries[i] = (vx*ax + vy*ay + vz*az) / std::max(speed, eps);
                }
                plotScalar = true;
            }
            break;
        }
        default: {
            // Displacement: vector components directly
            dataToVisualize = data.channelsData[channelIndex];

            // Preserve the precompute for single-axis velocity lookup elsewhere
            if (dimCount == 1) {
                std::vector<posData> velocityData =
                    SignalProcessing::calculateVelocity(
                        data.channelsData[channelIndex], data.wavSR);
                std::vector<double> result;
                if (!config.name.isEmpty()) {
                    QChar axis = config.name.at(config.name.size() - 1).toLower();
                    if (axis == 'x') { for (const auto &v : velocityData) result.push_back(v.x); }
                    else if (axis == 'y') { for (const auto &v : velocityData) result.push_back(v.y); }
                    else if (axis == 'z') { for (const auto &v : velocityData) result.push_back(v.z); }
                    precomputedVelocity[config.name] = result;
                }
            }

            // precompute non-directional |v_xy| for this base sensor/config
            {
                const auto v = SignalProcessing::calculateVelocity(
                    data.channelsData[channelIndex], data.wavSR);
                std::vector<double> speedXY;
                speedXY.reserve(v.size());
                for (const auto& vi : v) {
                    speedXY.push_back(std::sqrt(vi.x * vi.x + vi.y * vi.y));
                }
                // Key by the exact config name so Label can ask with whatever it knows;
                // also store by base sensor name to make lookup flexible.
                precomputedSpeedXY[config.name] = speedXY;
                const QString baseKey = baseConfigName.trimmed();
                if (!baseKey.isEmpty()) precomputedSpeedXY[baseKey] = speedXY;
            }

            break;
        }
        }

        // ---- Make a visualizer
        KinematicVisualizer *visualizer = new KinematicVisualizer(this);
        visualizer->setConfigName(config.name);
        visualizer->setSizePolicy(sizePolicy);
        visualizer->setMinimumHeight(fixedHeight);

        // Use 3D / derived color for this config
        const QColor cfgColor = colorForConfig(config.name);
        visualizer->setBaseColor(cfgColor);

        const bool invertThis = hasInvertPrefix(config.name);

        QMap<QString, QVector<double>> posDataMap;

        if (plotScalar) {
            if (invertThis) {
                for (int i = 0; i < scalarSeries.size(); ++i) scalarSeries[i] = -scalarSeries[i];
            }
            // Put scalar on "X" so Track X works
            posDataMap["X"] = scalarSeries;
        } else {
            if (invertThis) {
                for (auto &s : dataToVisualize) {
                    if (config.x) s.x = -s.x;
                    if (config.y) s.y = -s.y;
                    if (config.z) s.z = -s.z;
                }
            }
            QVector<double> posX(static_cast<int>(dataToVisualize.size())),
                posY(static_cast<int>(dataToVisualize.size())),
                posZ(static_cast<int>(dataToVisualize.size()));
            for (size_t i = 0; i < dataToVisualize.size(); ++i) {
                posX[int(i)] = dataToVisualize[i].x;
                posY[int(i)] = dataToVisualize[i].y;
                posZ[int(i)] = dataToVisualize[i].z;
            }
            if (config.x) posDataMap["X"] = posX;
            if (config.y) posDataMap["Y"] = posY;
            if (config.z) posDataMap["Z"] = posZ;
        }

        visualizer->visualizeSignal(posDataMap, config.name, 2, data.wavSR);
        ui->posPlotVL->addWidget(visualizer);
        kinematicVisualizers.append(visualizer);

        if (Label *label = visualizer->getLabel()) {
            connect(label, &Label::landmarkAdded, this, &mainWindow::onLandmarkAddedFromLabel);
            connect(label, &Label::labelClicked,  this, &mainWindow::onLabelClicked);
            connect(label, &Label::labelMoved,    this, &mainWindow::onLabelMoved, Qt::DirectConnection);
            m_allLabels.push_back(label);

            label->setVelocityThresholdFraction(m_appliedVelocityThresholdFraction);
            label->setReopeningDeadbandFraction(m_appliedReopeningDeadbandFraction);
        }
    }

    if (!kinematicVisualizers.isEmpty()) {
        xAxisMinLimit = kinematicVisualizers.first()->getXAxisMinLimit();
        xAxisMaxLimit = kinematicVisualizers.first()->getXAxisMaxLimit();
        updateScrollBar();
    }
}

void mainWindow::addLandmarkToModel(const QString &channel,
                                    double offset,
                                    const QString &lblName)
{
    QString finalName = lblName;
    if (finalName.isEmpty())
        finalName = QStringLiteral("name%1").arg(globalNameCounter++);

    QString updatedChannel = channel;
    int index = updatedChannel.indexOf(" (");
    if (index != -1)
        updatedChannel = channel.left(index);

    const double yValue = getLandmarkYValue(updatedChannel, offset);

    QList<QStandardItem *> items;
    items << new QStandardItem(finalName)
          << new QStandardItem(updatedChannel)
          << new QStandardItem(QString::number(offset, 'f', 3))
          << new QStandardItem(std::isfinite(yValue)
                                   ? QString::number(yValue, 'f', 3)
                                   : QString());

    items[1]->setFlags(items[1]->flags() & ~Qt::ItemIsEditable);
    items[2]->setFlags(items[2]->flags() & ~Qt::ItemIsEditable);
    items[3]->setFlags(items[3]->flags() & ~Qt::ItemIsEditable);

    landmarkListModel->appendRow(items);
    enableConfigureTabButtons(true);

    QModelIndex newIndex = landmarkListModel->index(landmarkListModel->rowCount() - 1, 0);
    ui->landmarkListView->setCurrentIndex(newIndex);
    ui->landmarkListView->selectionModel()->select(
        newIndex,
        QItemSelectionModel::Select | QItemSelectionModel::Rows
        );
    ui->landmarkListView->scrollTo(newIndex);
}

// ------------------------ Scrollbar sync ------------------------

void mainWindow::updateScrollBar()
{
    if (!ui->rangeScrollBar)
        return;

    QSignalBlocker blocker(ui->rangeScrollBar);

    if (kinematicVisualizers.isEmpty()) {
        ui->rangeScrollBar->setVisible(false);
        ui->rangeScrollBar->setEnabled(false);
        return;
    }

    QCustomPlot *plot = kinematicVisualizers.first()->getCustomPlot();
    if (!plot) {
        ui->rangeScrollBar->setVisible(false);
        ui->rangeScrollBar->setEnabled(false);
        return;
    }

    const double currentRangeSize = plot->xAxis->range().size();
    const double fullRange = xAxisMaxLimit - xAxisMinLimit;

    if (fullRange <= 0.0 || currentRangeSize >= fullRange) {
        ui->rangeScrollBar->setVisible(false);
        ui->rangeScrollBar->setEnabled(false);
        return;
    }

    ui->rangeScrollBar->setVisible(true);
    ui->rangeScrollBar->setEnabled(true);

    const int precision = 1000;
    const int pageStep = static_cast<int>((currentRangeSize / fullRange) * precision);

    if (pageStep <= 0 || pageStep >= precision) {
        ui->rangeScrollBar->setVisible(false);
        ui->rangeScrollBar->setEnabled(false);
        return;
    }

    ui->rangeScrollBar->setPageStep(pageStep);
    ui->rangeScrollBar->setRange(0, precision - pageStep);

    const double currentCenter = plot->xAxis->range().center();
    const double minMovableCenter = xAxisMinLimit + currentRangeSize / 2.0;
    const double maxMovableCenter = xAxisMaxLimit - currentRangeSize / 2.0;

    int value = 0;
    if (maxMovableCenter > minMovableCenter) {
        const double normalizedValue =
            (currentCenter - minMovableCenter) / (maxMovableCenter - minMovableCenter);
        value = static_cast<int>(normalizedValue * (precision - pageStep));
    }

    value = std::clamp(value, 0, precision - pageStep);
    ui->rangeScrollBar->setValue(value);
}

void mainWindow::onRangeScrollBarValueChanged(int value)
{
    if (kinematicVisualizers.isEmpty())
        return;

    QCustomPlot *plot = kinematicVisualizers.first()->getCustomPlot();
    if (!plot)
        return;

    const double currentRangeSize = plot->xAxis->range().size();

    const int precision = 1000;
    const int pageStep = ui->rangeScrollBar->pageStep();

    const double minMovableCenter = xAxisMinLimit + currentRangeSize / 2.0;
    const double maxMovableCenter = xAxisMaxLimit - currentRangeSize / 2.0;

    if (precision - pageStep <= 0)
        return;

    const double normalizedValue =
        static_cast<double>(value) / (precision - pageStep);

    const double newCenter =
        minMovableCenter + normalizedValue * (maxMovableCenter - minMovableCenter);

    double newLower = newCenter - currentRangeSize / 2.0;
    double newUpper = newCenter + currentRangeSize / 2.0;

    if (newLower < xAxisMinLimit) {
        newLower = xAxisMinLimit;
        newUpper = newLower + currentRangeSize;
    }
    if (newUpper > xAxisMaxLimit) {
        newUpper = xAxisMaxLimit;
        newLower = newUpper - currentRangeSize;
    }

    plot->xAxis->setRange(newLower, newUpper);
    plot->xAxis2->setRange(newLower, newUpper);
    plot->replot(QCustomPlot::rpQueuedReplot);

    rebuildSpectrogramForVisibleRange();
}


// ------------------------ File Info ------------------------

void mainWindow::showFileInfoDialog()
{
    if (!currentSpan) {
        QMessageBox::warning(this,
                             tr("No file loaded"),
                             tr("Please load a SPAN file first."));
        return;
    }

    FileInfoDialog dlg(this);
    dlg.setSpanFile(*currentSpan);
    dlg.exec();   // modal; use dlg.show() if you prefer modeless
}

// ------------------------ File management ------------------------

void mainWindow::addSpanButtonClicked() {
    fileNames = QFileDialog::getOpenFileNames(this, "Select Files", lastOpenedDir, "All Files (*.*)");
    if (fileNames.isEmpty()) return;

    lastOpenedDir = QFileInfo(fileNames.last()).absolutePath();
    for (const QString &fileName : fileNames) {
        QFileInfo fileInfo(fileName);
        QString suffix = fileInfo.suffix().toLower();

        if (suffix != "span" && suffix != "csv") {
            QMessageBox::warning(this, "File Extension Error",
                                 "Only SPAN or CSV files can be selected. Found: " + suffix);
            continue;
        }

        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            file.close();
            QFileInfo fileInfo(fileName);

            QStandardItem *nameItem = new QStandardItem(fileInfo.fileName());
            nameItem->setData(fileName, Qt::UserRole);   // full path stored here
            nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);

            // Tooltip: full name (or full path if you prefer)
            nameItem->setData(fileInfo.fileName(), Qt::ToolTipRole);
            // or: nameItem->setData(fileName, Qt::ToolTipRole); // full path

            // Format timestamp
            const QString ts = fileInfo.lastModified().toString("yyyy-MM-dd HH:mm:ss");
            QStandardItem *timeItem = new QStandardItem(ts);
            timeItem->setFlags(timeItem->flags() & ~Qt::ItemIsEditable);
            timeItem->setData(fileInfo.lastModified(), Qt::UserRole);

            // Tooltip: full timestamp
            timeItem->setData(ts, Qt::ToolTipRole);

            // Type column ("SPAN" / "CSV" etc.)
            QString typeStr = suffix.toUpper();
            QStandardItem *typeItem = new QStandardItem(typeStr);
            typeItem->setFlags(typeItem->flags() & ~Qt::ItemIsEditable);

            QList<QStandardItem*> row;
            row << nameItem << typeItem << timeItem;
            spanFilesListModel->appendRow(row);
            spanFilesNames.push_back(fileName);


        } else {
            QMessageBox::warning(this, "File Open Error",
                                 "Cannot open the file: " + fileName);
        }
    }
}

// ------------------------ Audio I/O ------------------------

void mainWindow::togglePlay()
{
    if (m_isPlaying.load(std::memory_order_acquire)) {
        stopPlayback();
        return;
    }
    if (currentSpan && !currentSpan->audioData.empty() && currentSpan->wavSR > 0) {
        playSoundButtonClicked(); // plays full file; Shift+Space handles selection
    }
}

struct AudioData {
    std::vector<float> samples;
    size_t position = 0;
    std::atomic<bool>* stopFlag = nullptr;
};

static int paCallback(const void * /*inputBuffer*/,
                      void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* /*timeInfo*/,
                      PaStreamCallbackFlags /*statusFlags*/,
                      void *userData)
{
    auto *audioData = static_cast<AudioData*>(userData);
    auto *out = static_cast<float*>(outputBuffer);

    if (audioData->stopFlag && audioData->stopFlag->load(std::memory_order_relaxed)) {
        for (unsigned long i = 0; i < framesPerBuffer; ++i) *out++ = 0.0f;
        return paComplete;
    }

    unsigned long framesLeft = framesPerBuffer;
    while (framesLeft > 0 && audioData->position < audioData->samples.size()) {
        *out++ = audioData->samples[audioData->position++];
        --framesLeft;
    }
    while (framesLeft-- > 0) *out++ = 0.0f;

    return (audioData->position < audioData->samples.size()) ? paContinue : paComplete;
}

void mainWindow::playSound(const std::vector<float> &audioData, int sampleRate) {
    PaError err;
    PaStream *stream = nullptr;
    AudioData *data = nullptr;
    bool paInited = false;

    m_stopRequested.store(false, std::memory_order_relaxed);

    auto cleanup = [&]{
        if (stream) {
            Pa_AbortStream(stream);
            while (Pa_IsStreamActive(stream) == 1) Pa_Sleep(5);
            Pa_CloseStream(stream);
            stream = nullptr;
        }
        m_paStream.store(nullptr, std::memory_order_release);
        if (paInited) {
            Pa_Terminate();
            paInited = false;
        }
        delete data;
        data = nullptr;
    };

    err = Pa_Initialize();
    if (err != paNoError) { cleanup(); return; }
    paInited = true;

    data = new AudioData{audioData, 0, &m_stopRequested};

    err = Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, sampleRate, 256, paCallback, data);
    if (err != paNoError) { cleanup(); return; }

    m_paStream.store(stream, std::memory_order_release);

    err = Pa_StartStream(stream);
    if (err != paNoError) { cleanup(); return; }

    while (Pa_IsStreamActive(stream) == 1) Pa_Sleep(50);
    cleanup();
}

void mainWindow::playSoundAsync(std::vector<float> audioData, int sampleRate)
{
    if (sampleRate <= 0 || audioData.empty()) return;

    if (m_isPlaying.load(std::memory_order_acquire)) {
        stopPlayback();
        for (int i = 0; i < 60 && m_isPlaying.load(std::memory_order_acquire); ++i)
            QThread::msleep(5);
    }

    bool expected = false;
    if (!m_isPlaying.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
        return;

    std::thread([this, audio = std::move(audioData), sr = sampleRate]() mutable {
        this->playSound(audio, sr);
        m_isPlaying.store(false, std::memory_order_release);
    }).detach();
}

void mainWindow::stopPlayback() {
    m_stopRequested.store(true, std::memory_order_relaxed);
    if (PaStream* s = m_paStream.exchange(nullptr, std::memory_order_acq_rel)) {
        Pa_AbortStream(s);
    }
    if (m_sensor3dTimer) {
        m_sensor3dTimer->stop();
    }
    m_playbackDurationSec = 0.0;
}



static inline void applyFadeInOut(std::vector<float>& buf, int sampleRate, double ms = 3.0)
{
    if (buf.empty() || sampleRate <= 0) return;
    int n = static_cast<int>(ms * 1e-3 * sampleRate);
    n = std::max(1, std::min(n, int(buf.size()/2)));
    for (int i = 0; i < n; ++i) {
        float g = float(i + 1) / float(n);
        buf[i] *= g;
        buf[buf.size() - 1 - i] *= g;
    }
}

void mainWindow::playSoundButtonClicked() {
    if (!currentSpan || currentSpan->audioData.empty() || currentSpan->wavSR <= 0) return;

    auto audio = currentSpan->audioData;
    applyFadeInOut(audio, currentSpan->wavSR);

    // drive 3D animation from t = 0 for the whole file
    m_playbackStartTimeSec = 0.0;
    m_playbackDurationSec =
        static_cast<double>(currentSpan->audioData.size()) / currentSpan->wavSR;

    m_playbackClock.restart();
    if (m_sensor3dTimer) m_sensor3dTimer->start();

    playSoundAsync(std::move(audio), currentSpan->wavSR);
}


void mainWindow::playSelectionButtonClicked() {
    if (!currentSpan || currentSpan->audioData.empty() || currentSpan->wavSR <= 0) return;

    double selectionStart = std::numeric_limits<double>::max();
    double selectionEnd   = std::numeric_limits<double>::lowest();
    bool hasSelection = false;

    for (auto visualizer : kinematicVisualizers) {
        QCPRange range = visualizer->getSelectionRange();
        if (range.lower < range.upper) {
            selectionStart = std::min(selectionStart, range.lower);
            selectionEnd   = std::max(selectionEnd,   range.upper);
            hasSelection = true;
        }
    }

    if (!hasSelection) {
        for (auto visualizer : kinematicVisualizers) {
            QCPRange range = visualizer->getCustomPlot()->xAxis->range();
            selectionStart = std::min(selectionStart, range.lower);
            selectionEnd   = std::max(selectionEnd,   range.upper);
        }
    }

    if (!(selectionStart < selectionEnd) || selectionStart < 0.0) return;

    const int sr = currentSpan->wavSR;
    const auto &audio = currentSpan->audioData;

    int startSample = static_cast<int>(std::floor(selectionStart * sr));
    int endSampleEx = static_cast<int>(std::ceil (selectionEnd   * sr));
    startSample = std::max(0, startSample);
    endSampleEx = std::min<int>(static_cast<int>(audio.size()), endSampleEx);

    if (endSampleEx - startSample <= 0) return;

    std::vector<float> selectedAudioData(audio.begin() + startSample,
                                         audio.begin() + endSampleEx);
    applyFadeInOut(selectedAudioData, sr);

    // absolute start of this segment in the file, and its duration
    m_playbackStartTimeSec = selectionStart;
    m_playbackDurationSec  = selectionEnd - selectionStart;

    m_playbackClock.restart();
    if (m_sensor3dTimer) m_sensor3dTimer->start();

    playSoundAsync(std::move(selectedAudioData), sr);
}


// ------------------------ Files list ops ------------------------

void mainWindow::removeSelectedSpanFiles() {
    auto *sel = ui->filesListView->selectionModel();
    if (!sel) return;

    QModelIndexList selectedRows = sel->selectedRows();  // <--- rows only
    if (selectedRows.isEmpty()) return;

    std::sort(selectedRows.begin(), selectedRows.end(),
              [](const QModelIndex &a, const QModelIndex &b) {
                  return a.row() > b.row();   // remove from bottom up
              });

    for (const QModelIndex &index : selectedRows) {
        int row = index.row();
        if (row >= 0 && row < static_cast<int>(spanFilesNames.size())) {
            spanFilesNames.erase(spanFilesNames.begin() + row);
        }
        spanFilesListModel->removeRow(row);
    }

    // The rest of your logic for currentSpan, clearAllPlots(), etc. stays as-is
    if (currentSpan) {
        auto it = std::find(spanFilesNames.begin(), spanFilesNames.end(),
                            QString::fromStdString(currentSpan->path));

        if (spanFilesListModel->rowCount() == 0 || it == spanFilesNames.end()) {
            stopPlayback();
            clearAllPlots();
            resetSensor3DView();

            delete currentSpan;
            currentSpan = nullptr;

            enableFilesTabButtons(false);
            enableConfigureTabButtons(false);
        }
    }
}

void mainWindow::updateRemoveSpanFilesButtonState(const QItemSelection &selected,
                                                  const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);

    auto *selModel = ui->filesListView->selectionModel();
    const bool hasSelection = selModel && selModel->hasSelection();

    ui->removeSpanFilesButton->setEnabled(hasSelection);
    ui->viewButton->setEnabled(hasSelection);
    ui->placeLabelsButton->setEnabled(hasSelection);
}

void mainWindow::clearAllPlots()
{
    KinematicVisualizer::clearSelectionRect();
    clearAllLandmarks();
    m_allLabels.clear();

    precomputedVelocity.clear();
    precomputedSpeedXY.clear();

    QLayoutItem* child = nullptr;
    clearLayout(ui->audioPlotVL);
    clearLayout(ui->posPlotVL);
    kinematicVisualizers.clear();

    configuredChannelsModel->clear();
    ui->fileNameLabel->clear();
    ui->X_checkBox->setChecked(false);
    ui->Y_checkBox->setChecked(false);
    ui->Z_checkBox->setChecked(false);
    ui->derivedChLineEdit->clear();

    ui->rangeScrollBar->setVisible(false);
    ui->rangeScrollBar->setEnabled(false);
}

void mainWindow::pushButtonClicked() {
    preprocessingWindow* window = new preprocessingWindow(this);
    connect(window, &preprocessingWindow::closed, this, &mainWindow::show);
    this->hide();
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();
}

// ------------------------ I/O of .span ------------------------

void mainWindow::initSensor3DMain()
{
    if (m_sensor3DMain)
        return;

    m_sensor3DMain = new Sensor3DVisualizer(this);

    // Only the 3D plot, no title / legend
    m_sensor3DMain->setupScatterPlot(
        ui->hl3DMAIN,
        nullptr,
        nullptr,
        QSize()    // invalid -> no enforced minimum
        );

    m_sensor3DMain->setGraphTitle(QString());

}

void mainWindow::populateSensor3DFromSpanFile(const spanFile &file)
{
    if (!m_sensor3DMain)
        initSensor3DMain();

    m_sensor3DColorMap.clear();

    int colorIndex = 0;

    // Assign a color to each *non-derived* sensor (skip id 0:audio and derived ids)
    for (const auto &entry : file.sensors) {
        int sensorId = entry.first;
        if (sensorId == 0) // audio
            continue;

        // Skip derived channels: IDs >= numChannels
        if (sensorId >= file.numChannels)
            continue;

        const QColor c = colorForIndex(colorIndex++);
        m_sensor3DColorMap[sensorId] = c;
    }

    updateSensor3DAt(0.0);
}


void mainWindow::updateSensor3DAt(double timeSeconds)
{
    if (!currentSpan || !m_sensor3DMain)
        return;

    const spanFile &file = *currentSpan;

    // Build a frame: one point per sensor at timeSeconds
    std::vector<channel> frameChannels;

    for (const auto &entry : file.sensors) {
        int sensorId = entry.first;
        const std::string &nameStd = entry.second;

        // Skip audio
        if (sensorId == 0 ||
            nameStd == "audio" || nameStd == "Audio")
            continue;

        // Skip derived channels: IDs >= numChannels
        if (sensorId >= file.numChannels)
            continue;

        int channelIndex = sensorId - 1;
        if (channelIndex < 0 ||
            channelIndex >= static_cast<int>(file.channelsData.size()))
            continue;

        const auto &posSeries = file.channelsData[static_cast<size_t>(channelIndex)];
        if (posSeries.empty())
            continue;

        // Convert time (seconds) -> sample index, clamped to this series
        size_t idx = 0;
        if (timeSeconds > 0.0) {
            double sampleIdxF = timeSeconds * file.wavSR;  // channelsData already interpolated to wavSR
            if (sampleIdxF < 0.0) sampleIdxF = 0.0;
            size_t last = posSeries.size() > 0 ? posSeries.size() - 1 : 0;
            if (sampleIdxF > static_cast<double>(last))
                sampleIdxF = static_cast<double>(last);
            idx = static_cast<size_t>(sampleIdxF);
        }

        const posData &p = posSeries[idx];

        channel ch;
        ch.sensorNumber = sensorId;
        ch.sensorName   = QString::fromStdString(nameStd);

        // Copy that single sample into channel::samples
        sampleData sd{};
        sd.x     = static_cast<float>(p.x);
        sd.y     = static_cast<float>(p.y);
        sd.z     = static_cast<float>(p.z);
        sd.phi   = static_cast<float>(p.phi);
        sd.theta = static_cast<float>(p.theta);
        sd.rms   = static_cast<float>(p.rms);
        sd.extra = 0.0f;      // if you have 'extra' in sampleData

        ch.samples.push_back(sd);

        // Color
        if (m_sensor3DColorMap.contains(sensorId))
            ch.color = m_sensor3DColorMap[sensorId];
        else
            ch.color = Qt::white;

        frameChannels.push_back(std::move(ch));
    }

    // Update 3D view with this frame: one point per sensor
    m_sensor3DMain->updateData(frameChannels);
    m_sensor3DMain->createLegend(frameChannels);

    m_sensor3DMain->setGraphTitle(QString());
}

void mainWindow::resetSensor3DView()
{
    if (!m_sensor3DMain)
        return;

    // Empty frame: no points
    std::vector<channel> empty;
    m_sensor3DMain->updateData(empty);
    m_sensor3DMain->createLegend(empty);
    m_sensor3DMain->setGraphTitle(QString());

    m_sensor3DColorMap.clear();

    // Also stop any 3D animation
    if (m_sensor3dTimer)
        m_sensor3dTimer->stop();

    m_playbackDurationSec = 0.0;
}

void mainWindow::writeSPANToTextFile(const std::string &path, const spanFile &data) {
    std::ofstream outFile(path);
    if (!outFile.is_open()) return;

    outFile << "Wav Sampling Rate: " << data.wavSR << std::endl;
    outFile << "Pos Sampling Rate: " << data.posSR << std::endl;
    outFile << "Number of Channels: " << data.numChannels << std::endl;

    outFile << "Sensors:" << std::endl;
    for (const auto &sensor : data.sensors) {
        outFile << "Sensor " << sensor.first << ": " << sensor.second << std::endl;
    }

    outFile << "Audio Data:" << std::endl;
    for (const auto &sample : data.audioData) outFile << sample << " ";
    outFile << std::endl;

    outFile << "Channel Data:" << std::endl;
    for (size_t ch = 0; ch < data.channelsData.size(); ++ch) {
        outFile << "Channel " << ch + 1 << ":" << std::endl;
        for (const auto &sample : data.channelsData[ch]) {
            outFile << sample.x << " " << sample.y << " " << sample.z << " "
                    << sample.phi << " " << sample.theta << " " << sample.rms << std::endl;
        }
    }
    outFile.close();
}

// ------------------------ CSV -> Labels load ------------------------

void mainWindow::placeLabelsButtonClicked()
{
    if (!currentSpan) {
        QMessageBox::warning(this, "No Selection",
                             "Please select a SPAN file before placing labels.");
        return;
    }

    QModelIndex selectedIndex = ui->filesListView->currentIndex();
    if (!selectedIndex.isValid()) {
        QMessageBox::warning(this, "No CSV Selected",
                             "Please select a CSV file from the list.");
        return;
    }

    QModelIndex nameIndex = selectedIndex.sibling(selectedIndex.row(), 0);
    QString filePath = nameIndex.data(Qt::UserRole).toString();
    QString suffix   = QFileInfo(filePath).suffix().toLower();
    if (suffix != "csv") {
        QMessageBox::warning(this, "Not a CSV",
                             "The selected file is not a CSV file.");
        return;
    }

    QFile csvFile(filePath);
    if (!csvFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "File Error", "Cannot open file: " + filePath);
        return;
    }

    QTextStream in(&csvFile);
    QString header = in.readLine().trimmed();
    if (!header.contains("Name", Qt::CaseInsensitive)
        || !header.contains("Channel", Qt::CaseInsensitive)
        || !header.contains("Offset", Qt::CaseInsensitive))
    {
        QMessageBox::warning(this, "CSV Format Error",
                             "Header must be: Name,Channel,Offset");
        csvFile.close();
        return;
    }

    int validCount = 0;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList fields = line.split(',', Qt::SkipEmptyParts);
        if (fields.size() < 3) continue;

        QString csvLabelName = fields[0].trimmed();
        QString channel      = fields[1].trimmed();
        QString offStr       = fields[2].trimmed();

        bool ok = false;
        double offset = offStr.toDouble(&ok);
        if (!ok) continue;

        bool rowAlreadyExists = false;
        for (int r = 0; r < landmarkListModel->rowCount(); ++r) {
            QString existingChannel = landmarkListModel->item(r, 1)->text();
            double existingOffset   = landmarkListModel->item(r, 2)->text().toDouble();
            if (existingChannel == channel && qAbs(existingOffset - offset) < 1e-6) {
                rowAlreadyExists = true;
                break;
            }
        }
        if (rowAlreadyExists) continue;

        bool placedLabel = false;
        for (auto *visualizer : kinematicVisualizers) {
            if (visualizer->configName() == channel) {
                if (Label *lbl = visualizer->getLabel()) {
                    lbl->placeLabelAt(offset, csvLabelName);
                    validCount++;
                    placedLabel = true;
                    break;
                }
            }
        }
        Q_UNUSED(placedLabel);
    }

    csvFile.close();

    if (validCount == 0) {
        QMessageBox::information(this, "No Labels Placed",
                                 "No valid rows matched any current channel.");
    } else {
        enableConfigureTabButtons(true);
        for (auto *visualizer : kinematicVisualizers) {
            if (visualizer->getCustomPlot())
                visualizer->getCustomPlot()->replot(QCustomPlot::rpQueuedReplot);
        }
    }
}

void mainWindow::placeLandmark(const QString &channelName, double offset, const QString &labelName)
{
    Label *labelPtr = nullptr;
    for (auto *visualizer : kinematicVisualizers) {
        labelPtr = visualizer->getLabel();
        if (labelPtr) break;
    }
    if (!labelPtr) return;

    labelPtr->placeLabelAt(offset, QString());

    QList<QStandardItem *> items;
    QStandardItem *itemName    = new QStandardItem(labelName.isEmpty() ? "Unnamed" : labelName);
    QStandardItem *itemChannel = new QStandardItem(channelName);
    QStandardItem *itemOffset  = new QStandardItem(QString::number(offset, 'f', 3));
    itemChannel->setFlags(itemChannel->flags() & ~Qt::ItemIsEditable);
    itemOffset->setFlags(itemOffset->flags() & ~Qt::ItemIsEditable);
    items << itemName << itemChannel << itemOffset;
    landmarkListModel->appendRow(items);

    enableConfigureTabButtons(true);
}

// ------------------------ Double click ------------------------

void mainWindow::onSpanFileDoubleClicked(const QModelIndex &index) {
    ui->filesListView->setCurrentIndex(index);

    QModelIndex selectedIndex = ui->filesListView->currentIndex();
    if (!selectedIndex.isValid()) {
        QMessageBox::warning(this, "No Selection", "Please select a span file to view.");
        return;
    }

    QModelIndex nameIndex = selectedIndex.sibling(selectedIndex.row(), 0);
    QString filePath = nameIndex.data(Qt::UserRole).toString();
    QString suffix = QFileInfo(filePath).suffix().toLower();

    if (suffix == "span") {
        viewSpanFile();
        return;
    } else if (suffix == "csv") {
        placeLabelsButtonClicked();
        return;
    }
}

// ------------------------ View .span ------------------------

bool mainWindow::loadSpanIntoUi(const QString &filePath)
{
    const QString suffix = QFileInfo(filePath).suffix().toLower();
    if (suffix != "span")
        return false;

    // Cache landmarks for the file currently open in the UI, before clearing it
    cacheCurrentLandmarksForSession();

    // Clear old plots + landmarks + 3D view
    clearAllPlots();
    resetSensor3DView();

    delete currentSpan;
    currentSpan = new spanFile();

    if (!SpanIO::readSpanFile(filePath.toStdString(), *currentSpan)) {
        QMessageBox::warning(this, "File Error",
                             "Cannot read SPAN file:\n" + filePath);
        delete currentSpan;
        currentSpan = nullptr;
        enableFilesTabButtons(false);
        return false;
    }

    // Reset per-file color state and initialize 3D colors
    m_configColorCache.clear();
    m_derivedParents.clear();
    populateSensor3DFromSpanFile(*currentSpan);  // fills m_sensor3DColorMap and draws t=0 frame

    // Invalidate spectrogram cache for the new file
    m_specCacheValid = false;

    // 2D plots now use the same colors via colorForConfig()
    visualizeSignal(*currentSpan);
    updateComboBox();
    updateConfiguredChannelsListView();

    // Default tracking mode = Auto
    ui->trackXRadioButton->setChecked(false);
    ui->trackYRadioButton->setChecked(false);
    ui->trackZRadioButton->setChecked(false);
    ui->trackAutoRadioButton->setChecked(true);
    updateTrackedParameter();   // push "Auto" to all visualizers

    ui->fileNameLabel->setText(QFileInfo(filePath).fileName());
    enableFilesTabButtons(true);

    ui->rangeScrollBar->setVisible(false);
    ui->rangeScrollBar->setEnabled(false);

    reloadTemplatesForCurrentDir();

    // Restore any landmarks remembered for this file during this app session
    restoreSessionLandmarksForCurrentSpan();

    return true;
}

void mainWindow::viewSpanFile()
{
    // Enforce: exactly ONE selected file
    auto *selModel = ui->filesListView->selectionModel();
    if (!selModel) {
        QMessageBox::warning(this, "No Selection",
                             "Please select exactly one span file to view.");
        return;
    }

    // use selectedRows() now that it's a table
    const QModelIndexList selected = selModel->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "No Selection",
                             "Please select exactly one span file to view.");
        return;
    }
    if (selected.size() > 1) {
        QMessageBox::warning(this, "Multiple Selection",
                             "Please select only one span file to view.");
        return;
    }

    const QModelIndex selectedIndex = selected.first();
    if (!selectedIndex.isValid()) {
        QMessageBox::warning(this, "No Selection",
                             "Please select a span file to view.");
        return;
    }

    const QString filePath =
        selectedIndex.sibling(selectedIndex.row(), 0).data(Qt::UserRole).toString();
    const QString suffix = QFileInfo(filePath).suffix().toLower();
    if (suffix != "span") {
        QMessageBox::warning(this, "File Extension Error",
                             "Only SPAN files (.span) can be viewed. Selected: " + suffix);
        return;
    }

    // Centralised span-loading logic
    if (!loadSpanIntoUi(filePath)) {
        // loadSpanIntoUi is responsible for showing its own error messages
        return;
    }
}


void mainWindow::refreshSpanFileRow(const QString &filePath)
{
    QFileInfo fi(filePath);
    if (!fi.exists()) return;

    const QString ts = fi.lastModified().toString("yyyy-MM-dd HH:mm:ss");

    for (int row = 0; row < spanFilesListModel->rowCount(); ++row) {
        QModelIndex nameIndex = spanFilesListModel->index(row, 0);
        const QString pathInModel = nameIndex.data(Qt::UserRole).toString();
        if (pathInModel == filePath) {
            QModelIndex timeIndex = spanFilesListModel->index(row, 2);
            spanFilesListModel->setData(timeIndex, ts);
            // optional raw datetime
            spanFilesListModel->setData(timeIndex, fi.lastModified(), Qt::UserRole);
        }
    }
}

// ------------------------ UI updates ------------------------

void mainWindow::updateComboBox() {
    if (!currentSpan) {
        ui->channelsComboBox->setEnabled(false);
        ui->channelsComboBox->clear();

        ui->ch1ComboBox->setEnabled(false);
        ui->ch1ComboBox->clear();

        ui->ch2ComboBox->setEnabled(false);
        ui->ch2ComboBox->clear();

        ui->channelTypeComboBox->setEnabled(false);
    } else {
        ui->channelsComboBox->setEnabled(true);
        ui->channelsComboBox->clear();

        ui->ch1ComboBox->setEnabled(true);
        ui->ch1ComboBox->clear();

        ui->ch2ComboBox->setEnabled(true);
        ui->ch2ComboBox->clear();

        ui->channelTypeComboBox->setEnabled(true);

        ui->channelsComboBox->addItem(QString());
        ui->ch1ComboBox->addItem(QString());
        ui->ch2ComboBox->addItem(QString());

        for (const auto &s : currentSpan->sensors) {
            const QString name = QString::fromStdString(s.second).trimmed();
            if (name.compare("audio", Qt::CaseInsensitive) == 0) continue;
            ui->channelsComboBox->addItem(name);
            ui->ch1ComboBox->addItem(name);
            ui->ch2ComboBox->addItem(name);
        }
    }
}

void mainWindow::updateConfiguredChannelsListView() {
    configuredChannelsModel->clear();
    if (!currentSpan) return;

    for (const auto &config : currentSpan->configurations) {
        QStandardItem *item = new QStandardItem(config.name);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        configuredChannelsModel->appendRow(item);
    }
}

void mainWindow::onConfiguredChannelSelected(const QItemSelection &selected,
                                             const QItemSelection &deselected) {
    Q_UNUSED(deselected);
    if (selected.indexes().isEmpty()) return;

    QModelIndex selectedIndex = selected.indexes().first();
    QString selectedChannelName = selectedIndex.data().toString();
    if (!currentSpan) return;

    ui->channelsComboBox->setCurrentIndex(0);

    ui->ch1ComboBox->setEnabled(false);
    ui->ch2ComboBox->setEnabled(false);
    ui->derivedChLineEdit->setEnabled(false);
    ui->channelTypeComboBox->setEnabled(false);
    ui->deriveButton->setEnabled(false);

    auto it = std::find_if(currentSpan->configurations.begin(), currentSpan->configurations.end(),
                           [&selectedChannelName](const SensorConfig &config) {
                               return config.name == selectedChannelName;
                           });

    if (it != currentSpan->configurations.end()) {
        ui->X_checkBox->setChecked(it->x);
        ui->Y_checkBox->setChecked(it->y);
        ui->Z_checkBox->setChecked(it->z);

        switch (it->param) {
        case motionParam::Displacement: ui->displacementRB->setChecked(true); break;
        case motionParam::Velocity:     ui->velocityRB->setChecked(true);     break;
        case motionParam::Acceleration: ui->accelerationRB->setChecked(true); break;
        default:
            radioButtonGroup->setExclusive(false);
            ui->displacementRB->setChecked(false);
            ui->velocityRB->setChecked(false);
            ui->accelerationRB->setChecked(false);
            radioButtonGroup->setExclusive(true);
            break;
        }

        setButtonsEnabled(true);
        setUIElementsEnabled(true);
    } else {
        setButtonsEnabled(false);
        setUIElementsEnabled(false);
    }
}

void mainWindow::onRadioButtonClicked(QAbstractButton*)
{
    if (!currentSpan) return;

    // NEW: If user is preparing to ADD a channel (channelsComboBox != empty),
    // do NOT mutate the selected configured channel.
    if (ui->channelsComboBox->currentIndex() > 0) {
        // Add mode → radios are only used by addConfigButtonClicked().
        return;
    }

    // Edit mode → ok to update the selected configured item.
    const auto idx = ui->configuredChannelsListView->currentIndex();
    if (!idx.isValid()) return;

    auto it = std::find_if(currentSpan->configurations.begin(),
                           currentSpan->configurations.end(),
                           [&](const SensorConfig& c){
                               return c.name.trimmed() == idx.data().toString().trimmed();
                           });
    if (it == currentSpan->configurations.end()) return;

    it->param =
        ui->velocityRB->isChecked()     ? motionParam::Velocity :
            ui->accelerationRB->isChecked() ? motionParam::Acceleration :
            motionParam::Displacement;
}


void mainWindow::moveUpButtonClicked() {
    QModelIndex currentIndex = ui->configuredChannelsListView->currentIndex();
    if (!currentIndex.isValid()) return;

    int currentRow = currentIndex.row();
    if (currentRow > 0) {
        QStandardItem *item = configuredChannelsModel->takeItem(currentRow);
        configuredChannelsModel->removeRow(currentRow);
        configuredChannelsModel->insertRow(currentRow - 1, item);
        ui->configuredChannelsListView->setCurrentIndex(configuredChannelsModel->index(currentRow - 1, 0));

        std::iter_swap(currentSpan->configurations.begin() + currentRow,
                       currentSpan->configurations.begin() + currentRow - 1);
    }
}

void mainWindow::moveDownButtonClicked() {
    QModelIndex currentIndex = ui->configuredChannelsListView->currentIndex();
    if (!currentIndex.isValid()) return;

    int currentRow = currentIndex.row();
    if (currentRow < configuredChannelsModel->rowCount() - 1) {
        QStandardItem *item = configuredChannelsModel->takeItem(currentRow);
        configuredChannelsModel->removeRow(currentRow);
        configuredChannelsModel->insertRow(currentRow + 1, item);
        ui->configuredChannelsListView->setCurrentIndex(configuredChannelsModel->index(currentRow + 1, 0));

        std::iter_swap(currentSpan->configurations.begin() + currentRow,
                       currentSpan->configurations.begin() + currentRow + 1);
    }
}

void mainWindow::removeConfigButtonClicked() {
    QModelIndex currentIndex = ui->configuredChannelsListView->currentIndex();
    if (!currentIndex.isValid()) return;

    QString selectedChannelName = currentIndex.data().toString();
    configuredChannelsModel->removeRow(currentIndex.row());

    auto it = std::find_if(currentSpan->configurations.begin(), currentSpan->configurations.end(),
                           [&selectedChannelName](const SensorConfig &config) {
                               return config.name == selectedChannelName;
                           });
    if (it != currentSpan->configurations.end()) currentSpan->configurations.erase(it);

    if (configuredChannelsModel->rowCount() == 0) {
        ui->moveUpButton->setEnabled(false);
        ui->moveDownButton->setEnabled(false);
        ui->removeConfigButton->setEnabled(false);
    } else {
        if (ui->configuredChannelsListView->selectionModel()->selectedIndexes().isEmpty()) {
            ui->moveUpButton->setEnabled(false);
            ui->moveDownButton->setEnabled(false);
        }
    }
}

void mainWindow::onChannelsComboBoxIndexChanged(int index) {
    if (index == 0) {
        ui->configuredChannelsListView->setEnabled(true);
        setButtonsEnabled(true);
    } else {
        ui->configuredChannelsListView->clearSelection();
        setButtonsEnabled(false);
        setUIElementsEnabled(true);

        ui->X_checkBox->setChecked(true);
        ui->Y_checkBox->setChecked(true);
        ui->Z_checkBox->setChecked(true);
        ui->displacementRB->setChecked(true);
    }
}

void mainWindow::updateConfigButtonClicked() {
    QModelIndex currentIndex = ui->configuredChannelsListView->currentIndex();
    if (!currentIndex.isValid()) return;

    bool xSelected = ui->X_checkBox->isChecked();
    bool ySelected = ui->Y_checkBox->isChecked();
    bool zSelected = ui->Z_checkBox->isChecked();

    if (!xSelected && !ySelected && !zSelected) {
        QMessageBox::warning(this, "Missing Information",
                             "Please ensure at least one parameter is selected.");
        return;
    }

    ui->trackXRadioButton->setChecked(false);
    ui->trackYRadioButton->setChecked(false);
    ui->trackZRadioButton->setChecked(false);
    ui->trackAutoRadioButton->setChecked(true);
    updateTrackedParameter();

    const QString originalChannelName = currentIndex.data().toString();
    const bool hadInvert = hasInvertPrefix(originalChannelName);

    QString motionPrefix;
    if (ui->velocityRB->isChecked())      motionPrefix = "v";
    else if (ui->accelerationRB->isChecked()) motionPrefix = "a";

    const QString baseName = baseSensorName(originalChannelName);
    QString constructedName = motionPrefix + baseName;

    if (!xSelected || !ySelected || !zSelected) {
        if (xSelected) constructedName += "x";
        if (ySelected) constructedName += "y";
        if (zSelected) constructedName += "z";
    }
    if (hadInvert) constructedName.prepend('i');

    configuredChannelsModel->setData(currentIndex, constructedName);

    auto it = std::find_if(currentSpan->configurations.begin(), currentSpan->configurations.end(),
                           [&originalChannelName](const SensorConfig &config) {
                               return config.name.trimmed() == originalChannelName.trimmed();
                           });

    if (it != currentSpan->configurations.end()) {
        it->name = constructedName;
        it->x = xSelected;
        it->y = ySelected;
        it->z = zSelected;
        if (motionPrefix == "v")      it->param = motionParam::Velocity;
        else if (motionPrefix == "a") it->param = motionParam::Acceleration;
        else                          it->param = motionParam::Displacement;
    }
}

void mainWindow::addConfigButtonClicked() {
    QString channelName = ui->channelsComboBox->currentText();
    bool xSelected = ui->X_checkBox->isChecked();
    bool ySelected = ui->Y_checkBox->isChecked();
    bool zSelected = ui->Z_checkBox->isChecked();

    QString motionPrefix;
    if (ui->velocityRB->isChecked())      motionPrefix = "v";
    else if (ui->accelerationRB->isChecked()) motionPrefix = "a";

    QString constructedName = motionPrefix + channelName;

    if (!xSelected || !ySelected || !zSelected) {
        if (xSelected) constructedName += "x";
        if (ySelected) constructedName += "y";
        if (zSelected) constructedName += "z";
    }

    QStandardItem *item = new QStandardItem(constructedName);
    configuredChannelsModel->appendRow(item);

    SensorConfig config;
    config.name = constructedName;
    config.x = xSelected;
    config.y = ySelected;
    config.z = zSelected;
    if (motionPrefix == "v")      config.param = motionParam::Velocity;
    else if (motionPrefix == "a") config.param = motionParam::Acceleration;
    else                          config.param = motionParam::Displacement;

    currentSpan->configurations.push_back(config);

    ui->channelsComboBox->setCurrentIndex(0);
    ui->X_checkBox->setChecked(true);
    ui->Y_checkBox->setChecked(true);
    ui->Z_checkBox->setChecked(true);
    ui->displacementRB->setChecked(true);
}

void mainWindow::applyConfigToAllButtonClicked() {
    if (spanFilesNames.empty() || !currentSpan) return;
    const std::string recipeBody = SpanIO::readRecipeBody(currentSpan->path);

    for (const QString &fileName : spanFilesNames) {
        const QString suffix = QFileInfo(fileName).suffix().toLower();
        if (suffix != "span") continue;

        updateConfigLine(fileName.toStdString(), currentSpan->configurations);
        overwriteRecipeLine(fileName.toStdString(), recipeBody);
    }
}

void mainWindow::configureButtonClicked() {
    if (!currentSpan) return;

    if (landmarkListModel->rowCount() > 0) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Save Landmarks",
            "This will remove all landmarks. Save them before proceeding?\nContinue?",
            QMessageBox::Yes | QMessageBox::No
            );
        if (reply == QMessageBox::No) return;
    }

    ui->trackXRadioButton->setChecked(false);
    ui->trackYRadioButton->setChecked(false);
    ui->trackZRadioButton->setChecked(false);
    ui->trackAutoRadioButton->setChecked(true);
    updateTrackedParameter();
    updateConfigLine(currentSpan->path, currentSpan->configurations);

    clearAllPlots();
    visualizeSignal(*currentSpan);
    updateConfiguredChannelsListView();
    updateComboBox();

    ui->fileNameLabel->setText(QFileInfo(QString::fromStdString(currentSpan->path)).fileName());
    ui->rangeScrollBar->setVisible(false);
    ui->rangeScrollBar->setEnabled(false);
}

void mainWindow::setUIElementsEnabled(bool enabled) {
    ui->displacementRB->setEnabled(enabled);
    ui->velocityRB->setEnabled(enabled);
    ui->accelerationRB->setEnabled(enabled);
    ui->X_checkBox->setEnabled(enabled);
    ui->Y_checkBox->setEnabled(enabled);
    ui->Z_checkBox->setEnabled(enabled);
}

void mainWindow::setButtonsEnabled(bool enabled) {
    ui->moveUpButton->setEnabled(enabled);
    ui->moveDownButton->setEnabled(enabled);
    ui->removeConfigButton->setEnabled(enabled);
    ui->updateConfigButton->setEnabled(enabled);
    ui->addConfigButton->setEnabled(!enabled);
    ui->configureButton->setEnabled(enabled);
    ui->applyConfigToAllButton->setEnabled(enabled);
    ui->ch1ComboBox->setEnabled(enabled);
    ui->ch2ComboBox->setEnabled(enabled);
    ui->derivedChLineEdit->setEnabled(enabled);
    ui->deriveButton->setEnabled(enabled);
    ui->channelTypeComboBox->setEnabled(enabled);
}

// ------------------------ Label interactions ------------------------

void mainWindow::onLabelClicked(const QString &channelName, double offset)
{
    Label* sendingLabel = qobject_cast<Label*>(sender());
    if (!sendingLabel) return;

    for (Label* lbl : m_allLabels) {
        if (lbl != sendingLabel) lbl->clearSelectedLine();
    }

    for (int row = 0; row < landmarkListModel->rowCount(); ++row) {
        QString modelChannel = landmarkListModel->item(row, 1)->text();
        double modelOffset   = landmarkListModel->item(row, 2)->text().toDouble();

        if (modelChannel == channelName && qAbs(modelOffset - offset) < 1e-1) {
            ui->landmarkListView->clearSelection();
            ui->landmarkListView->selectRow(row);
            ui->landmarkListView->scrollTo(landmarkListModel->index(row, 0));
            break;
        }
    }
}

void mainWindow::onLandmarkSelectionChanged(const QItemSelection &selected,
                                            const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    if (selected.indexes().isEmpty()) return;

    QModelIndex currentIndex = selected.indexes().first();
    int row = currentIndex.row();
    QString channelName = landmarkListModel->item(row, 1)->text();
    double offset = landmarkListModel->item(row, 2)->text().toDouble();
    highlightLabelInPlot(channelName, offset);
}

void mainWindow::highlightLabelInPlot(const QString &channelName, double offset)
{
    for (Label* lbl : m_allLabels) lbl->clearSelectedLine();
    for (Label* lbl : m_allLabels) {
        if (lbl->selectLineIfMatch(channelName, offset)) break;
    }
}

void mainWindow::updateRemoveLandmarkButtonState()
{
    bool hasSelection = ui->landmarkListView->selectionModel()->hasSelection();
    ui->removeLandmarkButton->setEnabled(hasSelection);
}

void mainWindow::onLabelMoved(const QString &channelName,
                              const QString &labelName,
                              double newX,
                              double oldX)
{
    Q_UNUSED(labelName);
    const double EPS = 1e-3;

    // Primary: find the row that matches the SAME channel and the OLD offset.
    int targetRow = -1;
    for (int row = 0; row < landmarkListModel->rowCount(); ++row) {
        const QString chan = landmarkListModel->item(row, 1)->text();
        const double  off  = landmarkListModel->item(row, 2)->text().toDouble();
        if (chan == channelName && std::abs(off - oldX) < EPS) {
            targetRow = row;
            break;
        }
    }

    // Fallback (very unlikely): if not found, choose the best match among same channel+name,
    //    i.e., the one whose offset is closest to oldX. This avoids silent failure if rounding differs.
    if (targetRow == -1) {
        double bestDist = std::numeric_limits<double>::max();
        for (int row = 0; row < landmarkListModel->rowCount(); ++row) {
            const QString nm   = landmarkListModel->item(row, 0)->text();
            const QString chan = landmarkListModel->item(row, 1)->text();
            const double  off  = landmarkListModel->item(row, 2)->text().toDouble();
            if (chan == channelName && nm == labelName) {
                const double d = std::abs(off - oldX);
                if (d < bestDist) { bestDist = d; targetRow = row; }
            }
        }
    }

    if (targetRow != -1) {
        landmarkListModel->item(targetRow, 2)->setText(QString::number(newX, 'f', 3));

        const double newY = getLandmarkYValue(channelName, newX);
        if (landmarkListModel->columnCount() > 3 && landmarkListModel->item(targetRow, 3)) {
            landmarkListModel->item(targetRow, 3)->setText(
                std::isfinite(newY) ? QString::number(newY, 'f', 3) : QString()
                );
        }

        ui->landmarkListView->clearSelection();
        ui->landmarkListView->selectRow(targetRow);
        ui->landmarkListView->scrollTo(landmarkListModel->index(targetRow, 0));
    }
}

void mainWindow::onLandmarkItemChanged(QStandardItem *item)
{
    if (item->column() != 0) return;

    QString newName = item->text().trimmed();
    if (newName.isEmpty()) {
        QMessageBox::warning(this, "Invalid Name", "Landmark name cannot be empty. Reverting to default.");
        item->setText("empty");
        return;
    }

    int row = item->row();
    QString channel = landmarkListModel->item(row, 1)->text();
    double offset   = landmarkListModel->item(row, 2)->text().toDouble();

    for (Label* label : m_allLabels) {
        QCustomPlot* plot = label->getPlot();
        if (plot && plot->graphCount() > 0 && plot->graph(0)->name() == channel) {
            label->updateLabelName(offset, newName);
            break;
        }
    }
}

std::vector<double> mainWindow::getPrecomputedVelocity(const QString &legendName) {
    if (precomputedVelocity.contains(legendName))
        return precomputedVelocity[legendName];
    return {};
}


// New: return |v_xy| series for the given config/sensor name.
// If not cached yet, compute-once and cache under both the exact key and its base sensor key.
std::vector<double> mainWindow::getPrecomputedSpeedXY(const QString& configOrSensorName)
{
    if (configOrSensorName.isEmpty() || !currentSpan) return {};

    // Fast path: exact cache hit
    if (precomputedSpeedXY.contains(configOrSensorName))
        return precomputedSpeedXY[configOrSensorName];

    // Build a flexible base key (strip i/v/a/x/y/z)
    QString baseKey = baseSensorName(configOrSensorName);
    if (precomputedSpeedXY.contains(baseKey))
        return precomputedSpeedXY[baseKey];

    // Resolve the sensor/channel index
    int channelIndex = -1;
    for (const auto& s : currentSpan->sensors) {
        const QString sname = QString::fromStdString(s.second).trimmed();
        if (sname == baseKey.trimmed() || sname == configOrSensorName.trimmed()) {
            channelIndex = s.first - 1;
            break;
        }
    }
    if (channelIndex < 0
        || channelIndex >= static_cast<int>(currentSpan->channelsData.size()))
        return {};

    // Compute |v_xy|
    const auto v = SignalProcessing::calculateVelocity(
        currentSpan->channelsData[channelIndex], currentSpan->wavSR);
    std::vector<double> speedXY; speedXY.reserve(v.size());
    for (const auto& vi : v) speedXY.push_back(std::sqrt(vi.x * vi.x + vi.y * vi.y));

    // Cache under both keys
    precomputedSpeedXY[configOrSensorName] = speedXY;
    if (!baseKey.isEmpty()) precomputedSpeedXY[baseKey] = speedXY;

    return speedXY;
}

void mainWindow::onLandmarkAddedFromLabel(const QString &channel,
                                          double offset,
                                          const QString &lblName)
{
    Label* sendingLabel = qobject_cast<Label*>(sender());
    if (!sendingLabel) return;

    QString finalName = lblName;
    if (finalName.isEmpty()) finalName = QString("name%1").arg(globalNameCounter++);

    addLandmarkToModel(channel, offset, finalName);
    sendingLabel->updateLabelName(offset, finalName);
}

// ------------------------ Invert channel ------------------------

void mainWindow::invertButtonClicked()
{
    if (!currentSpan) return;

    QModelIndex currentIndex = ui->configuredChannelsListView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "No Selection",
                             "Please select a configured channel to invert.");
        return;
    }

    if (landmarkListModel->rowCount() > 0) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Save Landmarks",
            "Inverting will re-draw plots and remove current landmarks.\nExport first?",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
            );
        if (reply == QMessageBox::Yes) {
            getCSVFileButtonClicked();
        } else if (reply == QMessageBox::Cancel) {
            return;
        }
    }

    const QString originalName = currentIndex.data().toString().trimmed();
    const bool alreadyInverted = originalName.startsWith('i');
    QString newName = originalName;
    if (alreadyInverted) newName.remove(0, 1);
    else                 newName.prepend('i');

    configuredChannelsModel->setData(currentIndex, newName);

    auto it = std::find_if(currentSpan->configurations.begin(),
                           currentSpan->configurations.end(),
                           [&](const SensorConfig &cfg) {
                               return cfg.name.trimmed() == originalName;
                           });
    if (it != currentSpan->configurations.end()) it->name = newName;

    updateConfigLine(currentSpan->path, currentSpan->configurations);

    clearAllPlots();
    visualizeSignal(*currentSpan);
    updateConfiguredChannelsListView();
    updateComboBox();

    for (int r = 0; r < configuredChannelsModel->rowCount(); ++r) {
        if (configuredChannelsModel->item(r)->text().trimmed() == newName) {
            ui->configuredChannelsListView->setCurrentIndex(
                configuredChannelsModel->index(r, 0));
            break;
        }
    }

    QMessageBox::information(this, alreadyInverted ? "Inversion Removed" : "Inverted",
                             QString("Channel \"%1\" is now \"%2\".").arg(originalName, newName));
}

// ------------------------ Menus / tooltips ----------------------

void mainWindow::setupMenus()
{
    QMenuBar *mb = menuBar();
    mb->clear();

    auto addMenuAction =
        [this](QMenu *menu,
               const QString &text,
               const QKeySequence &shortcut,
               auto fn) -> QAction *
    {
        QAction *act = menu->addAction(text);
        if (!shortcut.isEmpty())
            act->setShortcut(shortcut);
        connect(act, &QAction::triggered, this, fn);
        return act;
    };

    // ---------------- File ----------------
    QMenu *fileMenu = mb->addMenu(tr("&File"));

    m_openAction = addMenuAction(fileMenu, tr("&Open..."),
                                 QKeySequence::Open,
                                 &mainWindow::addSpanButtonClicked);

    m_preprocessAction = addMenuAction(fileMenu, tr("&Preprocess..."),
                                       QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_P),
                                       &mainWindow::pushButtonClicked);

    fileMenu->addSeparator();

    m_viewFileAction = addMenuAction(fileMenu, tr("&View"),
                                     QKeySequence(),
                                     &mainWindow::viewSpanFile);

    m_fileInfoAction = addMenuAction(fileMenu, tr("File &Info"),
                                     QKeySequence(Qt::CTRL | Qt::Key_I),
                                     &mainWindow::showFileInfoDialog);

    fileMenu->addSeparator();

    m_exitAction = fileMenu->addAction(tr("E&xit"));
    m_exitAction->setShortcut(QKeySequence::Quit);
    connect(m_exitAction, &QAction::triggered, this, [this]() { close(); });

    // ---------------- Playback ----------------
    QMenu *playbackMenu = mb->addMenu(tr("&Playback"));

    m_playPauseAction = addMenuAction(playbackMenu, tr("&Play / Pause"),
                                      QKeySequence(Qt::Key_Space),
                                      &mainWindow::togglePlay);

    m_playSelectionAction = addMenuAction(playbackMenu, tr("Play &Selection"),
                                          QKeySequence(Qt::SHIFT | Qt::Key_Space),
                                          &mainWindow::playSelectionButtonClicked);

    m_stopAction = addMenuAction(playbackMenu, tr("&Stop"),
                                 QKeySequence(Qt::Key_Escape),
                                 &mainWindow::stopPlayback);

    // ---------------- View ----------------
    QMenu *viewMenu = mb->addMenu(tr("&View"));

    m_selectRangeAction = addMenuAction(viewMenu, tr("&Select Range"),
                                        QKeySequence(Qt::Key_S),
                                        &mainWindow::on_selectButton_clicked);

    m_zoomInAction = addMenuAction(viewMenu, tr("Zoom &In"),
                                   QKeySequence::ZoomIn,
                                   &mainWindow::zoomInButtonClicked);

    m_zoomOutAction = addMenuAction(viewMenu, tr("Zoom &Out"),
                                    QKeySequence::ZoomOut,
                                    &mainWindow::zoomOutButtonClicked);

    m_resetViewAction = addMenuAction(viewMenu, tr("&Reset View"),
                                      QKeySequence(Qt::CTRL | Qt::Key_0),
                                      &mainWindow::resetButtonClicked);

    // ---------------- Annotate ----------------
    QMenu *annotateMenu = mb->addMenu(tr("&Annotate"));

    m_exportCsvAction = addMenuAction(annotateMenu, tr("&Export CSV..."),
                                      QKeySequence(Qt::CTRL | Qt::Key_E),
                                      &mainWindow::getCSVFileButtonClicked);

    m_clearAllLandmarksAction = addMenuAction(annotateMenu, tr("&Clear All Landmarks"),
                                              QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_L),
                                              &mainWindow::clearAllLandmarks);

    m_placeLabelsAction = addMenuAction(annotateMenu, tr("Place &Labels"),
                                        QKeySequence(Qt::CTRL | Qt::Key_L),
                                        &mainWindow::placeLabelsButtonClicked);

    // ---------------- Help ----------------
    QMenu *helpMenu = mb->addMenu(tr("&Help"));

    m_helpAction = helpMenu->addAction(tr("SPAN &Help"));
    m_helpAction->setShortcut(QKeySequence::HelpContents);
    connect(m_helpAction, &QAction::triggered, this, &mainWindow::showHelpDialog);

    m_contactAction = helpMenu->addAction(tr("&Contact Us"));
    connect(m_contactAction, &QAction::triggered, this, [this]() {
        QDesktopServices::openUrl(
            QUrl("mailto:allen.shamsi@gmail.com"
                 "?subject=SPAN%20feedback"
                 "&body=Hi%20Allen,%0A%0A")
            );
    });

    m_aboutAction = helpMenu->addAction(tr("&About SPAN"));
    connect(m_aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::information(
            this,
            tr("About SPAN"),
            tr("SPAN\n\n"
               "Software for preprocessing, viewing, annotating, and exporting EMA-based speech data.\n\n"
               "For bug reports, suggestions, or other questions, contact: allen.shamsi@gmail.com")
            );
    });

    // Initial enabled state
    if (m_viewFileAction)          m_viewFileAction->setEnabled(false);
    if (m_placeLabelsAction)       m_placeLabelsAction->setEnabled(false);
    if (m_fileInfoAction)          m_fileInfoAction->setEnabled(false);

    if (m_playPauseAction)         m_playPauseAction->setEnabled(false);
    if (m_playSelectionAction)     m_playSelectionAction->setEnabled(false);
    if (m_stopAction)              m_stopAction->setEnabled(false);

    if (m_selectRangeAction)       m_selectRangeAction->setEnabled(false);
    if (m_zoomInAction)            m_zoomInAction->setEnabled(false);
    if (m_zoomOutAction)           m_zoomOutAction->setEnabled(false);
    if (m_resetViewAction)         m_resetViewAction->setEnabled(false);

    if (m_exportCsvAction)         m_exportCsvAction->setEnabled(false);
    if (m_clearAllLandmarksAction) m_clearAllLandmarksAction->setEnabled(false);

    // Always available
    if (m_openAction)              m_openAction->setEnabled(true);
    if (m_preprocessAction)        m_preprocessAction->setEnabled(true);
    if (m_exitAction)              m_exitAction->setEnabled(true);
    if (m_helpAction)              m_helpAction->setEnabled(true);
    if (m_contactAction)           m_contactAction->setEnabled(true);
    if (m_aboutAction)             m_aboutAction->setEnabled(true);

}

QString mainWindow::buildHelpHtml() const
{
    return QStringLiteral(R"(
        <h2>SPAN Help</h2>

        <p>
        SPAN is a desktop application for inspecting, configuring, annotating, and exporting
        articulatory speech data stored in <code>.span</code> files.
        In a typical workflow, the user loads one or more <code>.span</code> files,
        reviews the waveform and kinematic signals, configures channels for display,
        places or edits landmarks, optionally derives new channels,
        and exports landmark summaries or applies annotation templates across files.
        </p>

        <p>
        The main window is intended for analysis of already prepared <code>.span</code> files.
        If you are starting from raw EMA positional and audio recordings, use the
        <b>Preprocess</b> function first to convert raw input into the <code>.span</code> format.
        </p>

        <h3>1. Opening and loading files</h3>
        <ul>
          <li><b>Open</b>: add one or more <code>.span</code> files to the file list.</li>
          <li><b>View</b>: load the selected file into the waveform, kinematic, and related views.</li>
          <li><b>Preprocess</b>: open the preprocessing window to create <code>.span</code> files from raw positional and audio data.</li>
          <li><b>File Info</b>: inspect header information and metadata associated with the current file.</li>
        </ul>

        <p>
        Once a file is loaded, SPAN displays the audio waveform and the currently selected
        kinematic traces. Depending on the current file and analysis state, the window may also
        show saved plot configurations, derived-channel recipes, landmarks, and other analysis information.
        If a control or menu item is disabled, it usually means that no valid file is currently loaded
        or that the current file does not support that action.
        </p>

        <h3>2. Main analysis views</h3>
        <ul>
          <li><b>Waveform view</b>: shows the audio signal for the current file and supports playback and time-range selection.</li>
          <li><b>Spectrogram view</b>: displays the acoustic spectrogram using the current spectrogram settings.</li>
          <li><b>Kinematic plot area</b>: shows the currently configured channels as stacked traces.</li>
          <li><b>Landmark table</b>: lists the landmarks associated with the current file and allows inspection or editing.</li>
          <li><b>3D sensor view</b>: provides a spatial preview of the available sensor configuration.</li>
        </ul>

        <p>
        These views are linked. In practice, SPAN is designed so that waveform, kinematic traces,
        spectrogram, and landmarks can be interpreted together rather than in isolation.
        This is especially useful when articulatory landmarks must be evaluated in relation to both
        acoustic and spatial information.
        </p>

        <h3>3. Plot configuration</h3>
        <ul>
          <li>Choose a sensor or derived channel from the available channel list.</li>
          <li>Select the motion type:
            <ul>
              <li><b>displacement</b></li>
              <li><b>velocity</b></li>
              <li><b>acceleration</b></li>
            </ul>
          </li>
          <li>Select the dimensions to display:
            <ul>
              <li><b>X</b></li>
              <li><b>Y</b></li>
              <li><b>Z</b></li>
            </ul>
          </li>
          <li>Use <b>Add Config</b> to add the new configuration to the stacked kinematic display.</li>
          <li>Use <b>Configure</b> to load an existing configuration back into the controls for review or editing.</li>
          <li>Use <b>Update</b> to modify a selected configuration.</li>
          <li>Use <b>Remove</b> to delete a configuration from the current stack.</li>
          <li>Use <b>Move Up</b> and <b>Move Down</b> to reorder the traces vertically.</li>
          <li>Use <b>Apply Config to All</b> to copy the current configuration layout across all loaded files.</li>
        </ul>

        <p>
        Plot configurations determine which channels are visible and how they are organized in the
        main kinematic display. Because the stacked view is central to later annotation,
        it is usually best to settle on a stable configuration before detailed landmarking.
        Reordering traces can make complex multi-channel analyses easier to interpret.
        </p>

        <h3>4. Derived channels</h3>
        <ul>
          <li>Choose two input channels to combine.</li>
          <li>Enter a name for the new derived channel.</li>
          <li>Select a derivation type, such as:
            <ul>
              <li>subtracting one component from another</li>
              <li>computing Euclidean distance</li>
              <li>other supported signal combinations available in the interface</li>
            </ul>
          </li>
          <li>Click <b>Derive</b> to create the new signal.</li>
          <li>Use <b>Clear All Derived</b> to remove all derived channels and clear the recipe line.</li>
        </ul>

        <p>
        Derived channels are useful when the signal you want to analyze is not directly stored
        as a single raw sensor trajectory. They can be used for constriction proxies,
        inter-articulator distance measures, or other custom movement summaries derived from
        existing channels.
        </p>

        <h3>5. Navigation, selection, and playback</h3>
        <ul>
          <li><b>Play / Pause</b>: play or pause the audio associated with the current file.</li>
          <li><b>Play Selection</b>: play only the currently selected time interval.</li>
          <li><b>Stop</b>: stop playback immediately.</li>
          <li><b>Select Range</b>: enable manual time-range selection in the display.</li>
          <li><b>Zoom In</b> and <b>Zoom Out</b>: change the visible time scale.</li>
          <li><b>Reset View</b>: restore the default visible range.</li>
          <li>The horizontal scrollbar can be used to pan through the signal when the view is zoomed.</li>
        </ul>

        <p>
        Navigation and playback tools are intended to help the user inspect articulatory events
        at multiple time scales. Audio playback is particularly useful when comparing the waveform
        and spectrogram to the kinematic traces around candidate landmarks.
        </p>

        <h3>6. Landmark annotation</h3>
        <ul>
          <li><b>Right-click</b> on a trace to place a plain landmark label.</li>
          <li><b>Shift + right-click</b> on a single-channel trace to snap to the nearest extremum and place <code>MaxC</code>.</li>
          <li>Drag a landmark handle to adjust its timing directly in the plot.</li>
          <li>Landmarks appear in the landmark table and can also be reviewed or edited there.</li>
          <li>Use <b>Place Labels</b> to place selected landmark labels through the UI controls.</li>
          <li>Use <b>Remove Selected Landmark</b> or <b>Clear All Landmarks</b> to manage annotations.</li>
        </ul>

        <p>
        Landmark placement is one of the central functions of SPAN.
        Depending on the current signal and available motion information,
        landmarks can be placed manually or with assisted placement based on local extrema.
        The landmark table is intended to give a compact summary of the annotations currently present in the file.
        </p>

        <p>
        When appropriate velocity information is available, SPAN can derive additional
        gesture-related landmarks from a <code>MaxC</code> anchor, including
        <code>GONS</code>, <code>NONS</code>, <code>PVEL1</code>,
        <code>PVEL2</code>, <code>NOFFS</code>, and <code>GOFFS</code>.
        These labels are intended for analyses of gestural timing, movement coordination,
        and the internal temporal structure of articulatory events.
        </p>

        <h3>7. Landmark export</h3>
        <ul>
          <li>Use <b>Export CSV</b> to save landmark information from the current file.</li>
          <li>The export includes the landmark label, associated channel, time offset, and Y value.</li>
          <li>The exported table can be used for downstream statistical analysis, quality control, or batch summaries.</li>
        </ul>

        <p>
        Exported landmark files are intended to bridge the interactive annotation stage and
        later quantitative analysis. In practice, this makes it easier to annotate in SPAN
        while analyzing results later in R, Python, Excel, or other statistical tools.
        </p>

        <h3>8. Templates and DTW-based annotation transfer</h3>
        <ul>
          <li>Annotate an exemplar file first, typically by placing reliable <code>MaxC</code> landmarks.</li>
          <li>Use <b>Save Template</b> to save the exemplar pattern as a reusable template.</li>
          <li>Use <b>Import Template</b> to load a template from disk.</li>
          <li>Use <b>Apply Template</b> to annotate the current file using the selected template.</li>
          <li>Use <b>Batch Apply Template</b> to apply the same template across multiple files.</li>
          <li>Use <b>Defaults</b> in the template/DTW area to restore the recommended settings.</li>
        </ul>

        <p>
        Templates are intended to speed up repetitive annotation work and improve consistency
        across files with similar channel configurations and gesture patterns.
        They are especially useful when many files share a comparable articulatory structure
        and the user wants a more systematic starting point than fully manual landmark placement.
        </p>

        <h3>9. Spectrogram controls</h3>
        <ul>
          <li>Adjust the spectrogram window size.</li>
          <li>Adjust the displayed maximum frequency.</li>
          <li>Adjust the spectrogram dynamic range.</li>
          <li>Use <b>Apply</b> to rebuild the spectrogram for the current visible range.</li>
          <li>Use <b>Defaults</b> to restore the recommended spectrogram settings.</li>
        </ul>

        <p>
        The spectrogram view is intended to support joint interpretation of acoustic and articulatory structure.
        Changing the display settings can be useful when the user needs to inspect fine temporal detail,
        emphasize a particular frequency range, or improve visibility for specific recordings.
        </p>

        <h3>10. Troubleshooting</h3>
        <ul>
          <li>If a button or menu item is disabled, first make sure a valid <code>.span</code> file is loaded.</li>
          <li>If no plots appear, check that the current configuration includes at least one valid channel and dimension.</li>
          <li>If a configuration does not behave as expected, verify that the selected file actually contains the relevant channel.</li>
          <li>If template application does not behave as expected, confirm that the current file contains the channels required by the template.</li>
          <li>If CSV export is empty or incomplete, check whether landmarks have actually been placed in the current file.</li>
          <li>If a derived channel is missing or invalid, confirm that both source channels exist and that the selected derivation type is appropriate.</li>
        </ul>

        <h3>11. Useful shortcuts</h3>
        <ul>
          <li><code>⌘ O</code>: Open files</li>
          <li><code>⌘ Shift P</code>: Open preprocessing window</li>
          <li><code>⌘ I</code>: File info</li>
          <li><code>Enter</code>: View selected file</li>
          <li><code>⌘ L</code>: Place labels</li>
          <li><code>Space</code>: Play / pause</li>
          <li><code>Shift Space</code>: Play selection</li>
          <li><code>Esc</code>: Stop playback</li>
          <li><code>S</code>: Select range</li>
          <li><code>⌘ +</code>: Zoom in</li>
          <li><code>⌘ -</code>: Zoom out</li>
          <li><code>⌘ 0</code>: Reset view</li>
          <li><code>⌘ E</code>: Export CSV</li>
          <li><code>⌘ Shift L</code>: Clear all landmarks</li>
          <li><code>⌘ ?</code>: Open help</li>
        </ul>

        <h3>12. Support</h3>
        <p>
        For bug reports, suggestions, feature requests, or other questions related to SPAN,
        use the <b>Help</b> menu contact option or email:
        <code>allen.shamsi@gmail.com</code>
        </p>
    )");
}

void mainWindow::showHelpDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("SPAN Help"));
    dialog.resize(760, 560);

    auto *layout = new QVBoxLayout(&dialog);

    auto *browser = new QTextBrowser(&dialog);
    browser->setOpenExternalLinks(true);
    browser->setHtml(buildHelpHtml());
    layout->addWidget(browser);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    dialog.exec();
}


void mainWindow::setupTooltips()
{
    auto tt = [](QWidget *w, const QString &text) {
        if (w) w->setToolTip(text);
    };

    // Files
    tt(ui->addSpanFilesButton, "⌘ O");
    tt(ui->viewButton, "Display the current file.");
    tt(ui->preprocessButton, "Create .span files.");
    tt(ui->infoButton, "Show info for the current file.");

    // Playback / view
    tt(ui->playSoundButton, "Space");
    tt(ui->playSelectionButton, "Shift Space");
    tt(ui->stopSoundButton, "Esc");
    tt(ui->zoomInButton, "⌘ +");
    tt(ui->zoomOutButton, "⌘ -");
    tt(ui->resetButton, "⌘ 0");
    tt(ui->selectButton, "S");

    // Config tab
    tt(ui->configureButton, "Visualize configured channels.");
    tt(ui->addConfigButton, "Add a channel to the plot stack.");
    tt(ui->updateConfigButton, "Update the selected channel configuration.");
    tt(ui->removeConfigButton, "Remove the selected channel.");
    tt(ui->applyConfigToAllButton, "Apply the current configuration set to all loaded files.");

    tt(ui->displacementRB, "Plot displacement.");
    tt(ui->velocityRB, "Plot velocity.");
    tt(ui->accelerationRB, "Plot acceleration.");

    tt(ui->X_checkBox, "Include the X dimension.");
    tt(ui->Y_checkBox, "Include the Y dimension.");
    tt(ui->Z_checkBox, "Include the Z dimension.");

    // Derived channels
    tt(ui->ch1ComboBox, "First input channel.");
    tt(ui->ch2ComboBox, "Second input channel.");
    tt(ui->derivedChLineEdit, "Name of the new derived channel.");
    tt(ui->channelTypeComboBox, "Choose the operation used to derive a new channel.");
    tt(ui->deriveButton, "Create a derived channel.");
    tt(ui->clearAllDerivedButton, "Remove all derived channels.");

    // Landmarking
    tt(ui->placeLabelsButton, "Place selected landmark labels on the current plots.");
    tt(ui->removeLandmarkButton, "Remove the selected landmark.");
    tt(ui->clearAllLandmarkButton, "⌘ Shift L");
    tt(ui->getCSVFileButton, "⌘ E");

    // Templates / DTW
    if (ui->templateComboBox)
        tt(ui->templateComboBox, "Choose a template.");
    if (ui->saveTemplateButton)
        tt(ui->saveTemplateButton, "Save the current file's annotation pattern as a template.");
    if (ui->importTemplateButton)
        tt(ui->importTemplateButton, "Import a template JSON file.");
    if (ui->applyTemplateButton)
        tt(ui->applyTemplateButton, "Apply the selected template to the current file.");
    if (ui->batchApplyTemplateButton)
        tt(ui->batchApplyTemplateButton, "Apply the selected template to multiple files.");
    if (ui->dtwDefaultsButton)
        tt(ui->dtwDefaultsButton, "Restore recommended DTW settings.");

    // Spectrogram
    if (ui->specWindowMsSpinBox)
        tt(ui->specWindowMsSpinBox, "Spectrogram analysis window size (in ms).");
    if (ui->specViewMaxHzSpinBox)
        tt(ui->specViewMaxHzSpinBox, "Maximum displayed frequency.");
    if (ui->specDynamicRangeSpinBox)
        tt(ui->specDynamicRangeSpinBox, "Displayed spectrogram dynamic range in dB.");
    if (ui->specApplyButton)
        tt(ui->specApplyButton, "Rebuild the spectrogram for the current visible range.");
    if (ui->specDefaultsButton)
        tt(ui->specDefaultsButton, "Restore default spectrogram settings.");
}

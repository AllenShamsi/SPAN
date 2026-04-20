#include "preprocessingWindow.h"
#include "ui_preprocessingwindow.h"
#include "OptionManager.h"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QStandardItemModel>
#include <QFileInfo>
#include <QModelIndex>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QGridLayout>
#include <QSet>

#include <memory>
#include <fstream>
#include <algorithm>
#include <limits>
#include <cmath>
#include <unordered_map>

#include "ButterworthFilter.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QTextBrowser>

// ---------------------- Constructor / Destructor ----------------------

preprocessingWindow::preprocessingWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::preprocessingWindow>())
    , lastOpenedDir(QDir::homePath())
    , biteplate()
    , sensorVisualizer(std::make_unique<Sensor3DVisualizer>(this))
    , messagesModel(new QStringListModel(this))
    , proceedPressedFirstTime(false)
{
    ui->setupUi(this);

    // Put central widget into a scroll area (for small screens)
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(ui->centralWidget);
    setCentralWidget(scrollArea);

    // File buttons
    connect(ui->pushButton2, &QPushButton::clicked,
            this, &preprocessingWindow::pushButton2Clicked);
    connect(ui->pushButton3, &QPushButton::clicked,
            this, &preprocessingWindow::pushButton3Clicked);

    // Main control buttons
    connect(ui->cancelButton, &QPushButton::clicked,
            this, &preprocessingWindow::cancelButtonClicked);
    connect(ui->startButton, &QPushButton::clicked,
            this, &preprocessingWindow::startButtonClicked);
    connect(ui->proceedButton, &QPushButton::clicked,
            this, &preprocessingWindow::proceedButtonClicked);
    connect(ui->backButton, &QPushButton::clicked,
            this, &preprocessingWindow::backButtonClicked);
    connect(ui->backButton_2, &QPushButton::clicked,
            this, &preprocessingWindow::showHelpDialog);

    // Models for file lists
    model2 = new QStandardItemModel(this); // biteplate POS
    model3 = new QStandardItemModel(this); // trial POS/WAV

    ui->BPFileSelectListView->setModel(model2);
    ui->TrialFileSelectListView->setModel(model3);

    ui->BPFileSelectListView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->TrialFileSelectListView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    ui->removeButton->setEnabled(false);
    ui->proceedButton->setEnabled(false);
    ui->backButton->setEnabled(false);

    connect(ui->TrialFileSelectListView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this, &preprocessingWindow::updateRemoveButtonState);

    connect(ui->removeButton, &QPushButton::clicked,
            this, &preprocessingWindow::removeSelectedItemsFromList3);

    // 3D visualizer
    sensorVisualizer->setupScatterPlot(ui->hl3D,
                                       ui->plotTitleWidget,
                                       ui->plotLegendWidget,
                                       QSize(300, 200));

    // Options manager for channel names
    optionManager = std::make_unique<OptionManager>(this);

    // -------- Per-channel widgets (16 channels) --------
    checkBoxes = {
        ui->checkBox1,  ui->checkBox2,  ui->checkBox3,  ui->checkBox4,
        ui->checkBox5,  ui->checkBox6,  ui->checkBox7,  ui->checkBox8,
        ui->checkBox9,  ui->checkBox10, ui->checkBox11, ui->checkBox12,
        ui->checkBox13, ui->checkBox14, ui->checkBox15, ui->checkBox16
    };

    comboBoxes = {
        ui->comboBox1,  ui->comboBox2,  ui->comboBox3,  ui->comboBox4,
        ui->comboBox5,  ui->comboBox6,  ui->comboBox7,  ui->comboBox8,
        ui->comboBox9,  ui->comboBox10, ui->comboBox11, ui->comboBox12,
        ui->comboBox13, ui->comboBox14, ui->comboBox15, ui->comboBox16
    };

    Q_ASSERT(checkBoxes.size() == comboBoxes.size());

    // ---- Dedicated BP-L/BP-F/BP-R combos (add these widgets in Designer) ----
    bpLeftCombo  = ui->bpLeftCombo;   // BP-L
    bpFrontCombo = ui->bpFrontCombo;  // BP-F
    bpRightCombo = ui->bpRightCombo;  // BP-R

    auto initBpCombo = [this](QComboBox *combo) {
        if (!combo) return;
        combo->clear();
        combo->addItem("Select", -1);
        combo->setEnabled(false);
        connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &preprocessingWindow::onBpRoleComboChanged);
    };

    initBpCombo(bpLeftCombo);
    initBpCombo(bpFrontCombo);
    initBpCombo(bpRightCombo);

    // Main channel checkboxes enable or disable the corresponding name selectors.
    for (int i = 0; i < checkBoxes.size(); ++i) {
        connect(checkBoxes[i], &QCheckBox::checkStateChanged,
                this, [this, i](Qt::CheckState state) {
                    onCheckBoxStateChanged(static_cast<int>(state), i);
                });
    }

    // Populate channel name combos
    for (int i = 0; i < comboBoxes.size(); ++i) {
        comboBoxes[i]->clear();
        comboBoxes[i]->setEditable(true);
        comboBoxes[i]->setInsertPolicy(QComboBox::NoInsert);
        comboBoxes[i]->setDuplicatesEnabled(false);

        for (const auto &opt : optionManager->getOptions()) {
            comboBoxes[i]->addItem(opt.first);
        }

        if (comboBoxes[i]->lineEdit()) {
            comboBoxes[i]->lineEdit()->setPlaceholderText("Select or type label");
        }

        connect(comboBoxes[i], QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this, i](int /*idx*/) {
                    onComboBoxChanged(i);
                });

        connect(comboBoxes[i]->lineEdit(), &QLineEdit::editingFinished,
                this, [this, i]() {
                    QComboBox *combo = comboBoxes[i];
                    QString text = combo->currentText().trimmed();

                    if (text.isEmpty()) {
                        combo->setCurrentText("Select");
                        return;
                    }

                    if (combo->findText(text) == -1) {
                        combo->addItem(text);
                    }

                    combo->setCurrentText(text);
                    refreshBpCombos();
                });

        previousSelections[i] = "Select";
    }

    // Keep options mutually exclusive (if OptionManager is used that way)
    connect(optionManager.get(), &OptionManager::optionAvailabilityChanged,
            this, [this](const QString &option, bool available) {
                for (auto *combo : comboBoxes) {
                    for (int i = 0; i < combo->count(); ++i) {
                        if (combo->itemText(i) == option) {
                            combo->setItemData(i,
                                               available ? QVariant() : QVariant(Qt::NoItemFlags),
                                               Qt::UserRole - 1);
                        }
                    }
                }
            });

    // Messages list
    ui->messagesListView->setModel(messagesModel);

    // Apply initial enable/disable state
    for (int i = 0; i < checkBoxes.size(); ++i) {
        onCheckBoxStateChanged(checkBoxes[i]->checkState(), i);
    }

    // No BP candidates at startup
    refreshBpCombos();

    setupTooltips();
}

preprocessingWindow::~preprocessingWindow() = default;

// ------------------------ UI helpers (combos / checkboxes) ------------------------

void preprocessingWindow::onComboBoxChanged(int index)
{
    QComboBox *comboBox = comboBoxes[index];
    QString selectedOption = comboBox->currentText().trimmed();
    QString previousOption = previousSelections[index];

    auto isBuiltInOption = [this](const QString &text) {
        for (const auto &opt : optionManager->getOptions()) {
            if (opt.first == text)
                return true;
        }
        return false;
    };

    // Re-enable previously selected built-in option in other combos
    if (previousOption != "Select" && isBuiltInOption(previousOption)) {
        optionManager->setOptionAvailability(previousOption, true);
    }

    // Disable newly selected built-in option in other combos
    if (selectedOption != "Select" && isBuiltInOption(selectedOption)) {
        optionManager->setOptionAvailability(selectedOption, false);
    }

    previousSelections[index] = selectedOption.isEmpty() ? "Select" : selectedOption;

    // Channel name changed -> update BP candidate dropdowns
    refreshBpCombos();
}

void preprocessingWindow::onCheckBoxStateChanged(int state, int index)
{
    const bool enabled = (state == Qt::Checked);

    if (index < 0 || index >= comboBoxes.size())
        return;

    comboBoxes[index]->setEnabled(enabled);

    if (!enabled) {
        int selectIdx = comboBoxes[index]->findText("Select");
        comboBoxes[index]->setCurrentIndex(selectIdx >= 0 ? selectIdx : 0);
    }

    // whenever a channel is enabled/disabled, recompute BP candidates
    refreshBpCombos();
}

void preprocessingWindow::onBpRoleComboChanged()
{
    // Keep bpSensors in sync with BP-L/BP-F/BP-R mapping
    syncBpSensorsFromCombos();
}

// Clears all per-channel checkboxes
void preprocessingWindow::resetSensors()
{
    for (auto *cb : checkBoxes)
        cb->setChecked(false);
}

// Build hrSensors + trialSensors from per-channel state
void preprocessingWindow::updateSensorsFromCheckboxes()
{
    hrSensors.clear();
    trialSensors.clear();

    const QSet<QString> refNames = {"REF-L", "REF-R", "REF-F"};

    for (int i = 0; i < checkBoxes.size(); ++i) {
        if (!checkBoxes[i]->isChecked())
            continue;

        const QString name = comboBoxes[i]->currentText().trimmed();
        if (name == "Select" || name.isEmpty())
            continue;

        const int channelIdx = i + 1;

        if (refNames.contains(name)) {
            hrSensors.insert(channelIdx, name);
        } else {
            trialSensors.insert(channelIdx, name);
        }
    }
}

// Rebuild BP role candidates from checked non-reference channels.
void preprocessingWindow::refreshBpCombos()
{
    struct Candidate {
        int channel;
        QString label;
    };

    QVector<Candidate> candidates;
    const QSet<QString> refNames = {"REF-L", "REF-R", "REF-F"};

    for (int i = 0; i < checkBoxes.size(); ++i) {
        if (!checkBoxes[i]->isChecked())
            continue;

        const QString name = comboBoxes[i]->currentText().trimmed();
        if (name == "Select" || name.isEmpty())
            continue;

        // REF channels cannot be BP channels
        if (refNames.contains(name))
            continue;

        int channelIdx = i + 1;
        Candidate c;
        c.channel = channelIdx;
        c.label = QString("ch %1: %2").arg(channelIdx).arg(name);
        candidates.push_back(c);
    }

    auto repopulate = [&](QComboBox *combo) {
        if (!combo)
            return;

        int previousChannel = combo->currentData(Qt::UserRole).toInt();

        combo->blockSignals(true);
        combo->clear();
        combo->addItem("Select", -1);
        for (const auto &c : candidates) {
            combo->addItem(c.label, c.channel);
        }
        combo->blockSignals(false);

        if (previousChannel > 0) {
            for (int i = 0; i < combo->count(); ++i) {
                if (combo->itemData(i, Qt::UserRole).toInt() == previousChannel) {
                    combo->setCurrentIndex(i);
                    break;
                }
            }
        }

        combo->setEnabled(combo->count() > 1);
    };

    repopulate(bpLeftCombo);
    repopulate(bpFrontCombo);
    repopulate(bpRightCombo);

    syncBpSensorsFromCombos();
}

void preprocessingWindow::syncBpSensorsFromCombos()
{
    bpSensors.clear();

    auto addRole = [this](QComboBox *combo, const QString &roleName) {
        if (!combo)
            return;
        int ch = combo->currentData(Qt::UserRole).toInt();
        if (ch > 0)
            bpSensors.insert(ch, roleName);
    };

    addRole(bpLeftCombo,  "BP-L");
    addRole(bpFrontCombo, "BP-F");
    addRole(bpRightCombo, "BP-R");
}

// ---------------- Push buttons for adding file names -----------------

void preprocessingWindow::pushButton2Clicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this, "Select POS File", lastOpenedDir, "All Files (*.*)");

    if (fileNames.isEmpty())
        return;

    if (fileNames.size() > 1) {
        QMessageBox::warning(this, "Limit Exceeded",
                             "You cannot select more than 1 file. Please select only 1 POS file.");
        return;
    }

    QString fileName = fileNames.first();
    QFileInfo fileInfo(fileName);
    QString suffix = fileInfo.suffix().toLower();

    if (suffix != "pos") {
        QMessageBox::warning(this, "File Extension Error",
                             "Only POS files can be selected. Found: " + suffix);
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "File Open Error",
                             "Cannot open the file: " + fileName);
        return;
    }
    file.close();

    lastOpenedDir = fileInfo.absolutePath();

    // Update the biteplate path and the model
    const std::string &oldPosPath = biteplate.getPosPath();
    if (!oldPosPath.empty()) {
        QString oldPath = QString::fromStdString(oldPosPath);
        for (int i = 0; i < model2->rowCount(); ++i) {
            QStandardItem *item = model2->item(i);
            if (item->data(Qt::UserRole).toString() == oldPath) {
                model2->removeRow(i);
                break;
            }
        }
    }

    biteplate.setPosPath(fileName.toStdString());

    QStandardItem *item = new QStandardItem(fileInfo.fileName());
    item->setData(fileName, Qt::UserRole);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    model2->appendRow(item);
}

void preprocessingWindow::pushButton3Clicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this, "Select Files", lastOpenedDir, "All Files (*.*)");

    if (fileNames.isEmpty())
        return;

    lastOpenedDir = QFileInfo(fileNames.last()).absolutePath();

    for (const QString &fileName : fileNames) {
        QFileInfo fileInfo(fileName);
        QString suffix = fileInfo.suffix().toLower();

        if (suffix != "pos" && suffix != "wav") {
            QMessageBox::warning(this, "File Extension Error",
                                 "Only POS or WAV files can be selected. Found: " + suffix);
            continue;
        }

        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            file.close();

            QStandardItem *item = new QStandardItem(fileInfo.fileName());
            item->setData(fileName, Qt::UserRole);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);

            model3->appendRow(item);
            trialFilesNames.push_back(fileName);
        } else {
            QMessageBox::warning(this, "File Open Error",
                                 "Cannot open the file: " + fileName);
        }
    }
}

// ------------- Methods for removing items from QListView -------------

void preprocessingWindow::removeSelectedItemsFromList3()
{
    QModelIndexList selectedIndexes =
        ui->TrialFileSelectListView->selectionModel()->selectedIndexes();

    std::sort(selectedIndexes.begin(), selectedIndexes.end(),
              [](const QModelIndex &a, const QModelIndex &b) {
                  return a.row() > b.row();
              });

    for (const QModelIndex &index : selectedIndexes) {
        trialFilesNames.erase(trialFilesNames.begin() + index.row());
        model3->removeRow(index.row());
    }
}

// ----------- Update remove button state based on selection -----------

void preprocessingWindow::updateRemoveButtonState(const QItemSelection &selected,
                                                  const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);

    bool hasSelection =
        !ui->TrialFileSelectListView->selectionModel()->selectedIndexes().isEmpty();
    ui->removeButton->setEnabled(hasSelection);
}

// --------------------------- Helper methods --------------------------


// ---- string trim ----
std::string preprocessingWindow::trim(const std::string &s)
{
    const auto first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return "";

    const auto last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}

// ---- POS reading ----
std::vector<channel> preprocessingWindow::readPosFile(const std::string &path)
{
    std::vector<channel> channels;
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        qWarning() << "Cannot open file:" << QString::fromStdString(path);
        return channels;
    }

    std::string headerLine;
    std::getline(file, headerLine);
    if (headerLine.substr(0, 11) == "AG50xDATA_V") {
        std::string versionStr = headerLine.substr(11, 3);
        if (versionStr == "002" || versionStr == "003") {
            std::string headerSizeStr;
            std::getline(file, headerSizeStr);
            int headerSize = std::stoi(trim(headerSizeStr));

            std::vector<char> headerBuffer(headerSize - headerLine.size() - headerSizeStr.size() - 2);
            file.read(headerBuffer.data(), headerBuffer.size());

            std::string numChannelsStr;
            std::string samplingFrequencyStr;
            std::string line;

            std::istringstream headerStream(std::string(headerBuffer.begin(), headerBuffer.end()));
            while (std::getline(headerStream, line)) {
                if (line.find("NumberOfChannels") != std::string::npos) {
                    numChannelsStr = line;
                } else if (line.find("SamplingFrequencyHz") != std::string::npos) {
                    samplingFrequencyStr = line;
                }
            }

            int numChannels = std::stoi(
                trim(numChannelsStr.substr(numChannelsStr.find('=') + 1)));
            posSR = std::stoi(
                trim(samplingFrequencyStr.substr(samplingFrequencyStr.find('=') + 1)));

            file.seekg(headerSize, std::ios::beg);
            if (!file.good()) {
                qWarning() << "Error seeking to the end of the POS header";
                return channels;
            }

            std::vector<char> binary_data((std::istreambuf_iterator<char>(file)),
                                          std::istreambuf_iterator<char>());
            size_t num_floats = binary_data.size() / sizeof(float);
            const float* float_data = reinterpret_cast<const float*>(binary_data.data());
            size_t num_samples =
                num_floats / (sampleData::num_members * numChannels);

            channels.resize(numChannels);
            for (int ch = 0; ch < numChannels; ++ch) {
                channels[ch].sensorNumber = ch + 1;
            }

            for (size_t sample = 0; sample < num_samples; ++sample) {
                for (int ch = 0; ch < numChannels; ++ch) {
                    sampleData data;
                    data.x = float_data[sample * sampleData::num_members * numChannels +
                                        ch * sampleData::num_members + 0];
                    data.y = float_data[sample * sampleData::num_members * numChannels +
                                        ch * sampleData::num_members + 1];
                    data.z = float_data[sample * sampleData::num_members * numChannels +
                                        ch * sampleData::num_members + 2];
                    data.phi = float_data[sample * sampleData::num_members * numChannels +
                                          ch * sampleData::num_members + 3];
                    data.theta = float_data[sample * sampleData::num_members * numChannels +
                                            ch * sampleData::num_members + 4];
                    data.rms = float_data[sample * sampleData::num_members * numChannels +
                                          ch * sampleData::num_members + 5];
                    data.extra = float_data[sample * sampleData::num_members * numChannels +
                                            ch * sampleData::num_members + 6];
                    channels[ch].samples.push_back(data);
                }
            }
        } else {
            qWarning() << "Unsupported file format version:" << QString::fromStdString(versionStr);
            return channels;
        }
    } else {
        qWarning() << "Invalid POS file format";
        return channels;
    }

    return channels;
}

// ---- color assignment ----
void preprocessingWindow::assignColors(std::vector<channel>& channels)
{
    QVector<QColor> colors = {
        QColor(Qt::red), QColor(Qt::magenta), QColor(Qt::yellow), QColor(Qt::blue),
        QColor(Qt::cyan), QColor(Qt::black), QColor(138, 43, 226), QColor(Qt::green),
        QColor(Qt::darkRed), QColor(Qt::darkGreen), QColor(Qt::darkBlue),
        QColor(Qt::darkYellow), QColor(Qt::darkCyan), QColor(Qt::darkMagenta),
        QColor(Qt::darkGray), QColor(Qt::lightGray),
        QColor(255, 165, 0), QColor(255, 20, 147), QColor(0, 255, 127), QColor(Qt::gray),
        QColor(210, 105, 30), QColor(240, 230, 140), QColor(50, 205, 50),
        QColor(255, 69, 0)
    };

    int colorIndex = 0;
    for (auto& ch : channels) {
        ch.color = colors[colorIndex % colors.size()];
        ++colorIndex;
    }
}

// ---- logical names for channels ----
void preprocessingWindow::assignChannelNames(bool isBP, std::vector<channel>& channels)
{
    for (auto& ch : channels) {
        if (isBP) {
            if (bpSensors.contains(ch.sensorNumber)) {
                ch.sensorName = bpSensors[ch.sensorNumber];
            } else if (hrSensors.contains(ch.sensorNumber)) {
                ch.sensorName = hrSensors[ch.sensorNumber];
            } else {
                ch.sensorName = "Unknown";
            }
        } else {
            if (trialSensors.contains(ch.sensorNumber)) {
                ch.sensorName = trialSensors[ch.sensorNumber];
            } else if (hrSensors.contains(ch.sensorNumber)) {
                ch.sensorName = hrSensors[ch.sensorNumber];
            } else {
                ch.sensorName = "Unknown";
            }
        }
    }
}

// ---- Biteplate correction ----
void preprocessingWindow::initializeOcclusalCorrectionModel(const std::vector<channel>& bitePlaneData,
                                                            const QMap<int, QString>& hrSensors,
                                                            const QMap<int, QString>& bpSensors)
{
    std::vector<int> refIdx;
    int refFKey = -1, refLKey = -1, refRKey = -1;

    for (auto it = hrSensors.begin(); it != hrSensors.end(); ++it) {
        if (it.value() == "REF-F") {
            refFKey = it.key();
        } else if (it.value() == "REF-L") {
            refLKey = it.key();
        } else if (it.value() == "REF-R") {
            refRKey = it.key();
        }
    }

    if (refFKey != -1) refIdx.push_back(refFKey - 1);
    if (refRKey != -1) refIdx.push_back(refRKey - 1);
    if (refLKey != -1) refIdx.push_back(refLKey - 1);

    std::vector<int> bpIdx;
    int bpLKey = -1, bpFKey = -1, bpRKey = -1;

    for (auto it = bpSensors.begin(); it != bpSensors.end(); ++it) {
        if (it.value() == "BP-L") {
            bpLKey = it.key();
        } else if (it.value() == "BP-F") {
            bpFKey = it.key();
        } else if (it.value() == "BP-R") {
            bpRKey = it.key();
        }
    }

    if (bpLKey != -1) bpIdx.push_back(bpLKey - 1);
    if (bpFKey != -1) bpIdx.push_back(bpFKey - 1);
    if (bpRKey != -1) bpIdx.push_back(bpRKey - 1);

    correctionModel = buildOcclusalCorrectionModel(bitePlaneData, refIdx, bpIdx);
    bpSensorData = applyOcclusalCorrection(correctionModel, bitePlaneData, refIdx);
}


std::vector<int> preprocessingWindow::selectedReferenceChannelIndices() const
{
    std::vector<int> refIdx;
    int refFKey = -1, refLKey = -1, refRKey = -1;

    for (auto it = hrSensors.begin(); it != hrSensors.end(); ++it) {
        if (it.value() == "REF-F") {
            refFKey = it.key();
        } else if (it.value() == "REF-L") {
            refLKey = it.key();
        } else if (it.value() == "REF-R") {
            refRKey = it.key();
        }
    }

    if (refFKey != -1) refIdx.push_back(refFKey - 1);
    if (refRKey != -1) refIdx.push_back(refRKey - 1);
    if (refLKey != -1) refIdx.push_back(refLKey - 1);

    return refIdx;
}

std::vector<channel> preprocessingWindow::getCorrectedTrial(
    const std::string &path,
    const std::unordered_set<int> &selectedKeys)
{
    std::vector<channel> trialData = readPosFile(path);
    std::vector<int> refIdx = selectedReferenceChannelIndices();

    std::vector<channel> correctedTrialData =
        applyOcclusalCorrection(correctionModel, trialData, refIdx);

    std::vector<channel> filteredData;
    for (const auto& ch : correctedTrialData) {
        if (selectedKeys.find(ch.sensorNumber) != selectedKeys.end()) {
            filteredData.push_back(ch);
        }
    }
    return filteredData;
}

// --------- Visualization of selected channels in 3D scatter ----------

void preprocessingWindow::visualizeSelectedChannels(
    bool isBP,
    const QMap<int, QString>& trialSensors,
    const QMap<int, QString>& bpSensors,
    const QMap<int, QString>& hrSensors)
{
    std::unordered_set<int> selectedKeys;
    std::vector<channel> neededSensorData;
    std::vector<channel> filteredData;

    for (auto it = hrSensors.begin(); it != hrSensors.end(); ++it)
        selectedKeys.insert(it.key());

    if (isBP) {
        for (auto it = bpSensors.begin(); it != bpSensors.end(); ++it)
            selectedKeys.insert(it.key());
        neededSensorData = bpSensorData;

        for (const auto& ch : neededSensorData) {
            if (selectedKeys.find(ch.sensorNumber) != selectedKeys.end()) {
                filteredData.push_back(ch);
            }
        }
    } else {
        for (auto it = trialSensors.begin(); it != trialSensors.end(); ++it)
            selectedKeys.insert(it.key());
        filteredData = getCorrectedTrial(trialFiles[0].getPosPath(), selectedKeys);
    }

    assignColors(filteredData);
    assignChannelNames(isBP, filteredData);
    sensorVisualizer->updateData(filteredData);
    sensorVisualizer->createLegend(filteredData);
}

// --------------------------- Input checks ----------------------------

bool preprocessingWindow::checkInput()
{
    // Rebuild HR + Trial maps from per-channel UI,
    // and BP map from the dedicated BP-L/BP-F/BP-R combos.
    updateSensorsFromCheckboxes();
    syncBpSensorsFromCombos();

    std::unordered_set<std::string> nameSet;

    trialFiles.clear();

    // --- Bite plane path checks ---
    if (biteplate.getPosPath().empty()) {
        QMessageBox::warning(this, "File Names Error",
                             "Bite Plane sensor selection is empty.");
        return false;
    }

    QFileInfo bpFileInfo(QString::fromStdString(biteplate.getPosPath()));
    QString bpRelativePath = bpFileInfo.fileName();
    bpRelativePath = bpRelativePath.left(bpRelativePath.lastIndexOf('.'));

    if (nameSet.find(bpRelativePath.toStdString()) != nameSet.end()) {
        QMessageBox::warning(this, "File Names Error",
                             "Cannot use the same file twice.\n Try again");
        return false;
    }
    biteplate.setName(bpRelativePath.toStdString());
    nameSet.insert(biteplate.getName());

    // --- Trial files basic checks ---
    if (trialFilesNames.empty()) {
        QMessageBox::warning(this, "File Names Error",
                             "Trial Files are empty");
        return false;
    }

    if (trialFilesNames.size() % 2 != 0) {
        QMessageBox::warning(this, "File Names Error",
                             "Must have even number of trial files");
        return false;
    }

    // --- Head reference sensors (REF-L, REF-R, REF-F) ---
    if (hrSensors.isEmpty()) {
        QMessageBox::warning(this, "Sensor Error",
                             "Please select head reference sensors");
        return false;
    }

    if (hrSensors.size() != 3) {
        QMessageBox::warning(this, "Head Reference Error",
                             "You must select exactly three checked channels named REF-L, REF-R, and REF-F.");
        return false;
    }

    {
        QSet<QString> requiredRefs = {"REF-L", "REF-R", "REF-F"};
        QSet<QString> foundRefs;

        for (auto it = hrSensors.cbegin(); it != hrSensors.cend(); ++it) {
            foundRefs.insert(it.value().trimmed());
        }

        if (foundRefs != requiredRefs) {
            QMessageBox::warning(this, "Head Reference Error",
                                 "Before proceeding, the checked sensor selection must include "
                                 "REF-L, REF-R, and REF-F exactly.");
            return false;
        }
    }

    {
        QStringList requiredSensors = {"REF-L", "REF-R", "REF-F"};
        for (const QString &sensor : requiredSensors) {
            bool found = false;
            for (auto it = hrSensors.cbegin(); it != hrSensors.cend(); ++it) {
                if (it.value() == sensor) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                QMessageBox::warning(this, "Missing Sensor Error",
                                     QString("Missing required reference sensor: %1").arg(sensor));
                return false;
            }
        }
    }

    // --- Biteplate sensors: from BP-L/BP-F/BP-R combos ---
    if (bpSensors.isEmpty()) {
        QMessageBox::warning(this, "Sensor Error",
                             "Please select bite plane channels (BP-L, BP-F, BP-R) "
                             "in the Bite Plane mapping section.");
        return false;
    }

    if (bpSensors.size() != 3) {
        QMessageBox::warning(this, "Sensor Number Error",
                             "You must select exactly 3 bite plane channels (BP-L, BP-F, BP-R).");
        return false;
    }

    std::unordered_set<int> bpChannels;
    bool hasBPL = false, hasBPF = false, hasBPR = false;
    for (auto it = bpSensors.cbegin(); it != bpSensors.cend(); ++it) {
        bpChannels.insert(it.key());
        if (it.value() == "BP-L") hasBPL = true;
        else if (it.value() == "BP-F") hasBPF = true;
        else if (it.value() == "BP-R") hasBPR = true;
    }

    if (bpChannels.size() != 3 || !hasBPL || !hasBPF || !hasBPR) {
        QMessageBox::warning(this, "Sensor Error",
                             "BP-L, BP-F and BP-R must be three different channels.");
        return false;
    }

    // --- Trial sensors: at least one, and no "Select" ---
    if (trialSensors.isEmpty()) {
        QMessageBox::warning(this, "Sensor Error",
                             "Please select trial sensors");
        return false;
    }

    for (auto it = trialSensors.cbegin(); it != trialSensors.cend(); ++it) {
        if (it.value() == "Select") {
            QMessageBox::warning(this, "Sensor Name Error",
                                 "Trial sensor name cannot be 'Select'.");
            return false;
        }
    }

    // --- File pairing logic (POS/WAV) ---
    std::unordered_map<std::string, QString> posFiles;
    std::unordered_map<std::string, QString> wavFiles;

    for (const auto& fileName : trialFilesNames) {
        QFileInfo fi(fileName);
        QString absolutePath = fi.absoluteFilePath();
        QString baseName = fi.baseName();
        QString extension = fi.suffix().toLower();

        if (extension == "pos") {
            posFiles[baseName.toStdString()] = absolutePath;
        } else if (extension == "wav") {
            wavFiles[baseName.toStdString()] = absolutePath;
        }
    }

    for (const auto& posFile : posFiles) {
        auto wavIt = wavFiles.find(posFile.first);
        if (wavIt == wavFiles.end()) {
            QMessageBox::warning(this, "File Names Error",
                                 QString::fromStdString("No WAV file for " + posFile.first));
            return false;
        }

        if (nameSet.find(posFile.first) != nameSet.end()) {
            QMessageBox::warning(this, "File Names Error",
                                 "Cannot use the same file twice.\n Try again");
            return false;
        }

        nameSet.insert(posFile.first);

        Sweep s;
        s.setPosPath(posFile.second.toStdString());
        s.setWavPath(wavIt->second.toStdString());
        s.setName(posFile.first);

        trialFiles.push_back(s);
        wavFiles.erase(wavIt);
    }

    if (!wavFiles.empty()) {
        QString unpaired;
        for (const auto& wavFile : wavFiles) {
            unpaired += QString::fromStdString(wavFile.first) + ", ";
        }
        QMessageBox::warning(this, "File Names Error",
                             "No POS file for WAV files: " + unpaired);
        return false;
    }

    return true;
}

// ----------------------------- Start / Proceed -----------------------

void preprocessingWindow::startButtonClicked()
{
    if (!checkInput())
        return;

    try {
        bpSensorData = readPosFile(biteplate.getPosPath());
        initializeOcclusalCorrectionModel(bpSensorData, hrSensors, bpSensors);
        visualizeSelectedChannels(true, trialSensors, bpSensors, hrSensors);
    } catch (const std::exception& e) {
        QMessageBox::warning(this,
                             "Preprocessing Error",
                             QString("Failed to compute occlusal correction:\n%1").arg(e.what()));
        return;
    }

    // UI state
    ui->startButton->setEnabled(false);
    ui->proceedButton->setEnabled(true);
    ui->backButton->setEnabled(true);

    updateGraphTitle("Bite Plane Plot");
    addMessage("Corrected bite plane channels have been visualized.");
}

void preprocessingWindow::proceedButtonClicked()
{
    if (!proceedPressedFirstTime) {
        sensorVisualizer->resetAxisBoundaries();
        visualizeSelectedChannels(false, trialSensors, bpSensors, hrSensors);

        updateGraphTitle("Experimental Data");

        QString trialSensorName = QString::fromStdString(trialFiles[0].getName());
        addMessage(QString("Trial data %1 have been visualized.").arg(trialSensorName));

        proceedPressedFirstTime = true;
        addMessage("Press 'Proceed' to convert all files.");
    } else {
        QString outPath = selectFolderPath();
        if (outPath.isEmpty())
            return;

        if (!outPath.endsWith(QDir::separator()))
            outPath += QDir::separator();

        std::unordered_set<int> selectedKeys;
        QMap<int, QString> selectedSensors;

        for (auto it = hrSensors.begin(); it != hrSensors.end(); ++it) {
            selectedKeys.insert(it.key());
            selectedSensors.insert(it.key(), it.value());
        }
        for (auto it = trialSensors.begin(); it != trialSensors.end(); ++it) {
            selectedKeys.insert(it.key());
            selectedSensors.insert(it.key(), it.value());
        }

        for (auto& sweep : trialFiles) {
            std::vector<channel> posData =
                getCorrectedTrial(sweep.getPosPath(), selectedKeys);
            std::vector<float> wavData = readWavFile(sweep.getWavPath());

            QString trialPathQStr =
                outPath + QString::fromStdString(sweep.getName()) + ".span";
            std::string trialPath = trialPathQStr.toStdString();

            writeSpanFile(trialPath, wavData, posData,
                          sweep.getPosPath(), selectedSensors);
        }

        QMessageBox::warning(this, "Success!",
                             "Your SPAN files have been saved. It is safe to close this window now.");
        resetWindow();
    }
}

// --------------------------- Reset window ----------------------------

void preprocessingWindow::resetWindow()
{
    // Clear file lists
    model2->removeRows(0, model2->rowCount());
    model3->removeRows(0, model3->rowCount());
    messagesModel->setStringList(QStringList());

    // Reset data
    trialFilesNames.clear();
    trialFiles.clear();
    hrSensors.clear();
    bpSensors.clear();
    trialSensors.clear();
    bpSensorData.clear();

    biteplate.setName("");
    biteplate.setWavPath("");
    biteplate.setPosPath("");

    // Reset per-channel widgets
    resetSensors();
    for (auto *comboBox : comboBoxes) {
        int idx = comboBox->findText("Select");
        comboBox->setCurrentIndex(idx >= 0 ? idx : 0);
    }

    // Reset BP mapping combos
    auto resetBpCombo = [](QComboBox *combo) {
        if (!combo) return;
        combo->blockSignals(true);
        combo->clear();
        combo->addItem("Select", -1);
        combo->setEnabled(false);
        combo->blockSignals(false);
    };
    resetBpCombo(bpLeftCombo);
    resetBpCombo(bpFrontCombo);
    resetBpCombo(bpRightCombo);

    sensorVisualizer->resetAxisBoundaries();
    sensorVisualizer->updateData(std::vector<channel>());
    sensorVisualizer->createLegend(std::vector<channel>());

    ui->startButton->setEnabled(true);
    ui->proceedButton->setEnabled(false);
    ui->removeButton->setEnabled(false);
    ui->backButton->setEnabled(false);

    proceedPressedFirstTime = false;
    updateGraphTitle("");
}

// ------------------------ Misc small helpers -------------------------

void preprocessingWindow::updateGraphTitle(const QString &newTitle)
{
    sensorVisualizer->setGraphTitle(newTitle);
}

void preprocessingWindow::addMessage(const QString &message)
{
    QStringList messages = messagesModel->stringList();
    messages.append(message);
    messagesModel->setStringList(messages);
}

// ---- WAV reading + normalization ----
std::vector<float> preprocessingWindow::readWavFile(const std::string &path)
{
    std::vector<float> samples;

    drwav wav;
    if (!drwav_init_file(&wav, path.c_str(), NULL)) {
        qWarning() << "Failed to open WAV file:" << QString::fromStdString(path);
        return samples;
    }

    wavSR = wav.sampleRate;

    std::vector<float> buffer(wav.totalPCMFrameCount * wav.channels);

    drwav_uint64 framesRead =
        drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, buffer.data());
    if (framesRead != wav.totalPCMFrameCount) {
        qWarning() << "Failed to read all PCM frames from WAV file";
        drwav_uninit(&wav);
        return samples;
    }

    if (wav.channels == 2) {
        for (drwav_uint32 frame = 0; frame < wav.totalPCMFrameCount; ++frame) {
            float leftSample = buffer[frame * 2];
            float rightSample = buffer[frame * 2 + 1];
            float monoSample = (leftSample + rightSample) / 2.0f;
            samples.push_back(monoSample);
        }
    } else if (wav.channels == 1) {
        samples.assign(buffer.begin(), buffer.begin() + wav.totalPCMFrameCount);
    } else {
        qWarning() << "Unsupported number of WAV channels:" << wav.channels;
        drwav_uninit(&wav);
        return samples;
    }

    drwav_uninit(&wav);

    std::vector<float> normalizeWavData = normalizeData(samples);
    return normalizeWavData;
}

// Normalize data to [-1, 1]
std::vector<float> preprocessingWindow::normalizeData(const std::vector<float>& data)
{
    float maxVal = 0.0f;
    for (float val : data) {
        maxVal = std::max(maxVal, std::abs(val));
    }

    if (maxVal == 0.0f)
        return data;

    std::vector<float> normalizedData;
    normalizedData.reserve(data.size());

    for (float val : data) {
        normalizedData.push_back(val / maxVal);
    }

    return normalizedData;
}

// ---- Header I/O for POS/SPAN ----
void preprocessingWindow::readOriginalPosHeader(const std::string &path,
                                                std::vector<std::string> &headerLines)
{
    std::ifstream inFile(path);
    if (!inFile.is_open()) {
        qDebug() << "Failed to open original POS file:" << path.c_str();
        return;
    }

    std::string line;
    for (int i = 0; i < 6 && std::getline(inFile, line); ++i) {
        headerLines.push_back(line);
    }
    inFile.close();
}

void preprocessingWindow::writeHeader(std::ofstream &outFile,
                                      const std::vector<std::string> &headerLines,
                                      int numberOfChannels,
                                      int wavSamplingFreq,
                                      int posSamplingFreq,
                                      const QMap<int, QString> &sensorLabels)
{
    for (int i = 0; i < static_cast<int>(headerLines.size()); ++i) {
        if (i == 2) {
            outFile << "channelsNum: " << numberOfChannels << "\n";
        } else if (i == 3) {
            outFile << "wavSR: " << wavSamplingFreq << "\n";
        } else if (i == 4) {
            outFile << "posSR: " << posSamplingFreq << "\n";
        } else {
            outFile << headerLines[i] << "\n";
        }
    }

    outFile << "0:audio ";
    for (auto it = sensorLabels.begin(); it != sensorLabels.end(); ++it) {
        outFile << it.key() << ":" << it.value().toStdString() << " ";
    }
    outFile << "\n";

    outFile << "Config: ";
    for (auto it = sensorLabels.begin(); it != sensorLabels.end(); ++it) {
        outFile << it.value().toStdString() << " ";
    }
    outFile << "\n";

    outFile << "Recipe: None\n";
}

void preprocessingWindow::writeSpanFile(const std::string &path,
                                        const std::vector<float> &wavData,
                                        std::vector<channel> &trialData,
                                        const std::string &originalPosHeaderPath,
                                        const QMap<int, QString> &selectedSensors)
{
    if (trialData.empty()) {
        QMessageBox::warning(this,
                             "Export Error",
                             "No positional channels are available for export.");
        return;
    }

    std::vector<std::string> headerLines;
    readOriginalPosHeader(originalPosHeaderPath, headerLines);

    std::ofstream outFile(path, std::ios::binary);
    if (!outFile.is_open()) {
        qDebug() << "Failed to open file for writing:" << path.c_str();
        return;
    }

    int totalChannels = static_cast<int>(trialData.size()) + 1;

    writeHeader(outFile, headerLines, totalChannels, wavSR, posSR, selectedSensors);
    outFile << "wavSamples: " << wavData.size() << "\n";
    outFile << "trialSamples: " << trialData[0].samples.size() << "\n";

    for (int i = 1; i < static_cast<int>(trialData.size()); ++i) {
        if (trialData[0].samples.size() != trialData[i].samples.size()) {
            QMessageBox::warning(this, "Sample Number Mismatch",
                                 "The number of samples in each positional channel does not match. "
                                 "The SPAN files will be saved with the number of samples in each "
                                 "positional channel set to "
                                     + QString::number(trialData[0].samples.size()) + ".");
            break;
        }
    }

    ButterworthFilter filter(20.0, posSR, 1);

    for (auto &ch : trialData) {
        std::vector<double> x_data(ch.samples.size());
        std::vector<double> y_data(ch.samples.size());
        std::vector<double> z_data(ch.samples.size());

        for (size_t i = 0; i < ch.samples.size(); ++i) {
            x_data[i] = ch.samples[i].x;
            y_data[i] = ch.samples[i].y;
            z_data[i] = ch.samples[i].z;
        }

        x_data = filter.filter(x_data);
        y_data = filter.filter(y_data);
        z_data = filter.filter(z_data);

        for (size_t i = 0; i < ch.samples.size(); ++i) {
            ch.samples[i].x = static_cast<float>(x_data[i]);
            ch.samples[i].y = static_cast<float>(y_data[i]);
            ch.samples[i].z = static_cast<float>(z_data[i]);
        }
    }

    // audio
    for (const auto &sample : wavData) {
        outFile.write(reinterpret_cast<const char*>(&sample), sizeof(float));
    }

    // positional
    for (const auto &ch : trialData) {
        for (const auto &sample : ch.samples) {
            outFile.write(reinterpret_cast<const char*>(&sample.x),     sizeof(float));
            outFile.write(reinterpret_cast<const char*>(&sample.y),     sizeof(float));
            outFile.write(reinterpret_cast<const char*>(&sample.z),     sizeof(float));
            outFile.write(reinterpret_cast<const char*>(&sample.phi),   sizeof(float));
            outFile.write(reinterpret_cast<const char*>(&sample.theta), sizeof(float));
            outFile.write(reinterpret_cast<const char*>(&sample.rms),   sizeof(float));
        }
    }

    outFile.close();
}

QString preprocessingWindow::selectFolderPath()
{
    QString folderPath = QFileDialog::getExistingDirectory(
        this, tr("Select Folder"), "",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    return folderPath;
}

// ------------------------ Closing / Back button ----------------------

void preprocessingWindow::cancelButtonClicked()
{
    close();
}

void preprocessingWindow::closeEvent(QCloseEvent *event)
{
    emit closed();
    QMainWindow::closeEvent(event);
}

void preprocessingWindow::backButtonClicked()
{
    messagesModel->setStringList(QStringList());

    sensorVisualizer->updateData(std::vector<channel>());
    sensorVisualizer->createLegend(std::vector<channel>());
    sensorVisualizer->resetAxisBoundaries();
    updateGraphTitle(QString());

    proceedPressedFirstTime = false;
    ui->proceedButton->setEnabled(false);
    ui->backButton->setEnabled(false);
    ui->startButton->setEnabled(true);
}

// ------------------------ Help / tooltips ----------------------

QString preprocessingWindow::buildHelpHtml() const
{
    return QStringLiteral(R"(
        <h2>Preprocessing Help</h2>

        <p>
        The preprocessing window converts raw EMA positional recordings and matching audio
        into <code>.span</code> files that can be opened in the main SPAN window.
        This workflow is intended to standardize the spatial frame of the data before
        visualization, annotation, and export.
        </p>

        <p>
        SPAN uses one bite-plane recording to define a participant-specific occlusal
        coordinate system. It then applies that same coordinate system to the experimental
        trial recordings using the three head-reference sensors. The result is a set of
        corrected trial files that share a consistent frame of reference within a participant.
        </p>

        <p>
        Current SPAN preprocessing is designed for raw EMA positional and audio files generated by the
        Carstens AG501 electromagnetic articulograph. In its current form, SPAN expects raw
        <code>.pos</code> files that contain three positional coordinates
        (<code>x</code>, <code>y</code>, <code>z</code>), two directional angles
        (<code>phi</code>, <code>theta</code>), and additional per-sample values such as
        <code>rms</code>, together with synchronized <code>.wav</code> audio recordings.
        </p>

        <h3>1. Select input files</h3>
        <ul>
          <li><b>Select BP File (.pos)</b>: choose one bite-plane <code>.pos</code> file.</li>
          <li><b>Select Trial Files</b>: choose the experimental trial files. These should include
              matching <code>.pos</code> and <code>.wav</code> files for each trial.</li>
          <li>Each trial <code>.pos</code> file must have a matching <code>.wav</code> file with the same base name.</li>
          <li>The file list shows which trial files are currently selected.</li>
          <li>Use <b>Remove</b> to remove selected trial files from the list before preprocessing.</li>
        </ul>

        <p>
        The bite-plane recording is used only to define the correction model.
        The trial recordings are then transformed into that same canonical frame and exported
        as new <code>.span</code> files.
        </p>

        <h3>2. Define the sensor layout</h3>
        <ul>
          <li>Check each raw channel that should participate in preprocessing.</li>
          <li>For every checked channel, choose or type a channel name.</li>
          <li>The three head-reference channels must be labeled exactly:
            <ul>
              <li><code>REF-L</code></li>
              <li><code>REF-R</code></li>
              <li><code>REF-F</code></li>
            </ul>
          </li>
          <li>Checked channels with those exact names are treated as the reference sensors used for frame-by-frame head-motion correction.</li>
          <li>All other checked and named channels are treated as trial channels and will be exported to the final <code>.span</code> files.</li>
        </ul>

        <p>
        This step is required because raw <code>.pos</code> files contain numbered channels,
        not meaningful anatomical labels. SPAN needs to know which raw channels correspond
        to the head-reference sensors and which correspond to the articulatory sensors of interest.
        </p>

        <h3>3. Assign bite-plane channels</h3>
        <ul>
          <li>Use the bite-plane mapping controls to assign three checked non-reference channels to:
            <ul>
              <li><code>BP-L</code></li>
              <li><code>BP-F</code></li>
              <li><code>BP-R</code></li>
            </ul>
          </li>
          <li>These must be three different channels.</li>
          <li>These channels define the participant-specific occlusal frame used during correction.</li>
        </ul>

        <p>
        SPAN computes the canonical coordinate system from the bite-plane geometry:
        <code>BP-L</code> and <code>BP-R</code> define the lateral axis,
        <code>BP-F</code> defines the anterior direction together with the left-right midpoint,
        and the origin is set to the centroid of <code>BP-L</code>, <code>BP-F</code>, and <code>BP-R</code>.
        The reference sensors are then expressed in that canonical frame and used as the target
        configuration for frame-by-frame correction of the bite-plane and trial recordings.
        </p>

        <h3>4. What the correction does</h3>
        <ul>
          <li>The bite-plane recording is used to build a participant-specific correction model.</li>
          <li>The correction model defines a canonical occlusal coordinate frame.</li>
          <li>The three reference sensors are used frame by frame to align each recording to that canonical frame.</li>
          <li>All selected positional channels are transformed into that corrected frame.</li>
          <li>The angular channels <code>phi</code> and <code>theta</code> are currently preserved unchanged.</li>
        </ul>

        <p>
        In other words, SPAN currently applies positional correction to the sensor coordinates
        used for visualization and export. It does not currently recompute angular sensor orientation
        values beyond preserving the original <code>phi</code> and <code>theta</code> values from the input.
        </p>

        <h3>5. Preview workflow</h3>
        <ul>
          <li><b>Start</b>:
            <ul>
              <li>validates all selected files and channel assignments</li>
              <li>reads the bite-plane file</li>
              <li>builds the occlusal correction model from the bite-plane and reference sensors</li>
              <li>applies the correction to the bite-plane recording</li>
              <li>shows the corrected bite-plane channels in the 3D preview</li>
            </ul>
          </li>
          <li><b>Proceed</b> (first click):
            <ul>
              <li>loads the first selected trial</li>
              <li>applies the same occlusal correction model to that trial</li>
              <li>shows the corrected trial channels in the 3D preview</li>
            </ul>
          </li>
          <li><b>Proceed</b> (second click):
            <ul>
              <li>asks for an output folder</li>
              <li>processes all valid trial pairs</li>
              <li>writes corrected <code>.span</code> files to disk</li>
            </ul>
          </li>
          <li><b>Back</b> clears the preview and returns the window to its initial state.</li>
          <li><b>Cancel</b> closes the preprocessing window.</li>
        </ul>

        <h3>6. 3D sensor preview</h3>
        <ul>
          <li>The <b>Sensor View</b> panel shows a corrected 3D preview of the currently loaded sensors.</li>
          <li>After <b>Start</b>, the preview shows the corrected bite-plane and reference configuration.</li>
          <li>After the first <b>Proceed</b>, the preview shows the corrected trial channels together with the selected references.</li>
          <li>The title and legend update to reflect the current preview stage.</li>
        </ul>

        <p>
        The preview is intended as a quick quality-control step before batch export.
        It helps confirm that the reference labels, bite-plane mapping, and selected trial channels
        produce a sensible corrected spatial configuration.
        </p>

        <h3>7. Output</h3>
        <ul>
          <li>Exported files are written as <code>.span</code> files.</li>
          <li>Each output file contains:
            <ul>
              <li>normalized audio</li>
              <li>corrected positional channel data</li>
              <li>the original <code>phi</code>, <code>theta</code>, and <code>rms</code> values</li>
              <li>header information derived from the original input</li>
              <li>the selected sensor labels used for export</li>
            </ul>
          </li>
          <li>Before export, the positional <code>x</code>, <code>y</code>, and <code>z</code> channels are smoothed with the current Butterworth filter settings implemented in SPAN.</li>
          <li>The output files can then be opened in the main SPAN window for plotting, annotation, template use, and CSV export.</li>
        </ul>

        <h3>8. Common input requirements</h3>
        <ul>
          <li>The bite-plane file must be a valid <code>.pos</code> file.</li>
          <li>Trial files must be provided in matching <code>.pos</code>/<code>.wav</code> pairs.</li>
          <li>The total number of trial files must be even, because each trial requires both a <code>.pos</code> and a <code>.wav</code>.</li>
          <li>The three reference labels must appear exactly once each:
            <code>REF-L</code>, <code>REF-R</code>, and <code>REF-F</code>.</li>
          <li>The bite-plane mapping must include exactly three distinct channels:
            <code>BP-L</code>, <code>BP-F</code>, and <code>BP-R</code>.</li>
          <li>At least one non-reference trial sensor must be selected.</li>
        </ul>

        <h3>9. Common errors and what they mean</h3>
        <ul>
          <li><b>Missing reference sensor</b>:
              one of <code>REF-L</code>, <code>REF-R</code>, or <code>REF-F</code> is missing or misspelled.</li>
          <li><b>BP-L / BP-F / BP-R must be different</b>:
              the same channel was assigned to more than one bite-plane role.</li>
          <li><b>No WAV file for ...</b>:
              a trial <code>.pos</code> file does not have a matching <code>.wav</code> file.</li>
          <li><b>No POS file for WAV files</b>:
              a trial <code>.wav</code> file does not have a matching <code>.pos</code> file.</li>
          <li><b>Trial sensor name cannot be Select</b>:
              a checked channel was left unnamed.</li>
          <li><b>Failed to compute occlusal correction</b>:
              the bite-plane or reference geometry was not valid for building a stable correction model. This can happen if required sensors are missing, duplicated, or geometrically degenerate.</li>
        </ul>

        <h3>10. Recommended workflow</h3>
        <ol>
          <li>Select the bite-plane file.</li>
          <li>Select all trial files.</li>
          <li>Check and label the raw channels.</li>
          <li>Confirm that the three reference channels are labeled
              <code>REF-L</code>, <code>REF-R</code>, and <code>REF-F</code>.</li>
          <li>Assign <code>BP-L</code>, <code>BP-F</code>, and <code>BP-R</code>.</li>
          <li>Click <b>Start</b> and inspect the corrected bite-plane preview.</li>
          <li>Click <b>Proceed</b> once and inspect the corrected trial preview.</li>
          <li>Click <b>Proceed</b> again to export all corrected files.</li>
        </ol>

        <h3>11. Support</h3>
        <p>
        If preprocessing fails unexpectedly or the corrected preview looks wrong,
        first re-check the reference labels, bite-plane mapping, and file pairing.
        If the geometry of the bite-plane or reference sensors is invalid, SPAN may stop
        before export rather than writing unusable output.
        For bug reports, suggestions, or related questions, contact:
        <code>allen.shamsi@gmail.com</code>
        </p>
    )");
}

void preprocessingWindow::showHelpDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Preprocessing Help"));
    dialog.resize(680, 500);

    auto *layout = new QVBoxLayout(&dialog);

    auto *browser = new QTextBrowser(&dialog);
    browser->setHtml(buildHelpHtml());
    layout->addWidget(browser);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    dialog.exec();
}


void preprocessingWindow::setupTooltips()
{
    auto tt = [](QWidget *w, const QString &text) {
        if (w) w->setToolTip(text);
    };

    // File section
    tt(ui->pushButton2, "Choose the bite-plane POS file.");
    tt(ui->pushButton3, "Choose trial POS and WAV files. Matching base names are required.");

    // Channel rows
    for (int i = 0; i < checkBoxes.size(); ++i) {
        tt(checkBoxes[i],
           QString("Enable raw channel %1 for preprocessing.").arg(i + 1));

        tt(comboBoxes[i],
           "Choose or type a channel label. ");
    }

    // Bite-plane role mapping
    tt(bpLeftCombo,  "Assign one checked channel to BP-L.");
    tt(bpFrontCombo, "Assign one checked channel to BP-F.");
    tt(bpRightCombo, "Assign one checked channel to BP-R.");

    // Control buttons
    tt(ui->startButton,
       "Validate inputs, compute bite-plane correction, and preview the corrected bite-plane channels.");
    tt(ui->proceedButton,
       "First click: preview corrected trial data. Second click: export all .span files.");
    tt(ui->backButton,
       "Return to the previous preprocessing state.");
    tt(ui->cancelButton,
       "Close the preprocessing window.");

}

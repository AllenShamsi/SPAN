/********************************************************************************
** Form generated from reading UI file 'mainWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mainWindow
{
public:
    QAction *actionOpen;
    QAction *actionOpen_Recent;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_7;
    QTabWidget *tabWidget;
    QWidget *filesTab;
    QWidget *verticalLayoutWidget_4;
    QVBoxLayout *verticalLayout_9;
    QTableView *filesListView;
    QPushButton *addSpanFilesButton;
    QPushButton *viewButton;
    QPushButton *removeSpanFilesButton;
    QPushButton *infoButton;
    QPushButton *placeLabelsButton;
    QPushButton *preprocessButton;
    QSpacerItem *verticalSpacer_3;
    QWidget *labelTab;
    QWidget *verticalLayoutWidget_6;
    QVBoxLayout *verticalLayout;
    QTableView *landmarkListView;
    QVBoxLayout *verticalLayout_11;
    QPushButton *getCSVFileButton;
    QPushButton *removeLandmarkButton;
    QPushButton *clearAllLandmarkButton;
    QGroupBox *templateGroupBox;
    QComboBox *templateComboBox;
    QSpinBox *dtwRateSpinBox;
    QSpinBox *dtwMaxLenSpinBox;
    QLabel *label_13;
    QLabel *label_14;
    QWidget *verticalLayoutWidget_8;
    QVBoxLayout *verticalLayout_12;
    QPushButton *saveTemplateButton;
    QPushButton *importTemplateButton;
    QWidget *verticalLayoutWidget_9;
    QVBoxLayout *verticalLayout_13;
    QPushButton *dtwDefaultsButton;
    QPushButton *applyTemplateButton;
    QPushButton *batchApplyTemplateButton;
    QWidget *configureTab;
    QComboBox *ch1ComboBox;
    QComboBox *ch2ComboBox;
    QLabel *label_9;
    QLabel *label_10;
    QLineEdit *derivedChLineEdit;
    QLabel *label_11;
    QComboBox *channelTypeComboBox;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *clearAllDerivedButton;
    QPushButton *deriveButton;
    QLabel *label_12;
    QGroupBox *groupBox_2;
    QComboBox *channelsComboBox;
    QCheckBox *X_checkBox;
    QLabel *label_3;
    QCheckBox *Y_checkBox;
    QCheckBox *Z_checkBox;
    QLabel *label_2;
    QRadioButton *displacementRB;
    QRadioButton *velocityRB;
    QRadioButton *accelerationRB;
    QWidget *horizontalLayoutWidget_3;
    QHBoxLayout *horizontalLayout;
    QPushButton *addConfigButton;
    QPushButton *updateConfigButton;
    QGroupBox *groupBox_5;
    QWidget *horizontalLayoutWidget_4;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *configureButton;
    QPushButton *applyConfigToAllButton;
    QWidget *verticalLayoutWidget_5;
    QVBoxLayout *verticalLayout_10;
    QPushButton *moveUpButton;
    QPushButton *moveDownButton;
    QPushButton *invertButton;
    QPushButton *removeConfigButton;
    QListView *configuredChannelsListView;
    QGroupBox *groupBox_6;
    QWidget *settingsTab;
    QGroupBox *groupBox_7;
    QLabel *label;
    QLabel *label_4;
    QLabel *label_5;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *specDefaultsButton;
    QPushButton *specApplyButton;
    QWidget *verticalLayoutWidget_10;
    QVBoxLayout *verticalLayout_14;
    QDoubleSpinBox *specViewMaxHzSpinBox;
    QDoubleSpinBox *specWindowMsSpinBox;
    QDoubleSpinBox *specDynamicRangeSpinBox;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *verticalLayout_5;
    QVBoxLayout *hl3DMAIN;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox;
    QWidget *horizontalLayoutWidget_6;
    QHBoxLayout *horizontalLayout_10;
    QPushButton *playSoundButton;
    QPushButton *playSelectionButton;
    QPushButton *stopSoundButton;
    QGroupBox *groupBox_3;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *zoomInButton;
    QPushButton *zoomOutButton;
    QPushButton *resetButton;
    QPushButton *selectButton;
    QGroupBox *groupBox_4;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_2;
    QRadioButton *trackXRadioButton;
    QRadioButton *trackYRadioButton;
    QRadioButton *trackZRadioButton;
    QRadioButton *trackAutoRadioButton;
    QVBoxLayout *verticalLayout_3;
    QLabel *fileNameLabel;
    QWidget *audioContainer;
    QVBoxLayout *verticalLayout_4;
    QVBoxLayout *audioPlotVL;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_8;
    QVBoxLayout *posPlotVL;
    QScrollBar *rangeScrollBar;

    void setupUi(QMainWindow *mainWindow)
    {
        if (mainWindow->objectName().isEmpty())
            mainWindow->setObjectName("mainWindow");
        mainWindow->resize(1008, 769);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mainWindow->sizePolicy().hasHeightForWidth());
        mainWindow->setSizePolicy(sizePolicy);
        mainWindow->setMinimumSize(QSize(0, 0));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush);
        QBrush brush1(QColor(255, 255, 255, 255));
        brush1.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush1);
        QBrush brush2(QColor(242, 242, 242, 255));
        brush2.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush2);
        QBrush brush3(QColor(115, 115, 115, 255));
        brush3.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush3);
        QBrush brush4(QColor(153, 153, 153, 255));
        brush4.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush4);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush1);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush1);
        QBrush brush5(QColor(243, 243, 243, 255));
        brush5.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush5);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush2);
        QBrush brush6(QColor(255, 255, 220, 255));
        brush6.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush6);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush);
        QBrush brush7(QColor(0, 0, 0, 127));
        brush7.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush7);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush1);
#endif
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush2);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush3);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush4);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush5);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush2);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush6);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush7);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush1);
#endif
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush3);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush2);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush3);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush4);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush3);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush3);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush5);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush5);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush);
        QBrush brush8(QColor(230, 230, 230, 255));
        brush8.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush8);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush6);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush);
        QBrush brush9(QColor(115, 115, 115, 127));
        brush9.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush9);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush1);
#endif
        mainWindow->setPalette(palette);
        mainWindow->setAnimated(false);
        mainWindow->setDocumentMode(false);
        mainWindow->setTabShape(QTabWidget::TabShape::Rounded);
        mainWindow->setDockOptions(QMainWindow::DockOption::AllowNestedDocks|QMainWindow::DockOption::AllowTabbedDocks);
        actionOpen = new QAction(mainWindow);
        actionOpen->setObjectName("actionOpen");
        actionOpen_Recent = new QAction(mainWindow);
        actionOpen_Recent->setObjectName("actionOpen_Recent");
        centralwidget = new QWidget(mainWindow);
        centralwidget->setObjectName("centralwidget");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy1);
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setSpacing(6);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(20, 20, 20, 20);
        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setSpacing(0);
        verticalLayout_7->setObjectName("verticalLayout_7");
        verticalLayout_7->setSizeConstraint(QLayout::SizeConstraint::SetNoConstraint);
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setEnabled(true);
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy2);
        tabWidget->setMinimumSize(QSize(300, 560));
        tabWidget->setMaximumSize(QSize(300, 16777215));
        QFont font;
        font.setFamilies({QString::fromUtf8(".AppleSystemUIFont")});
        tabWidget->setFont(font);
        tabWidget->setTabPosition(QTabWidget::TabPosition::North);
        tabWidget->setTabShape(QTabWidget::TabShape::Rounded);
        tabWidget->setMovable(false);
        filesTab = new QWidget();
        filesTab->setObjectName("filesTab");
        sizePolicy2.setHeightForWidth(filesTab->sizePolicy().hasHeightForWidth());
        filesTab->setSizePolicy(sizePolicy2);
        verticalLayoutWidget_4 = new QWidget(filesTab);
        verticalLayoutWidget_4->setObjectName("verticalLayoutWidget_4");
        verticalLayoutWidget_4->setGeometry(QRect(10, 0, 272, 531));
        verticalLayout_9 = new QVBoxLayout(verticalLayoutWidget_4);
        verticalLayout_9->setSpacing(0);
        verticalLayout_9->setObjectName("verticalLayout_9");
        verticalLayout_9->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        verticalLayout_9->setContentsMargins(0, 0, 0, 0);
        filesListView = new QTableView(verticalLayoutWidget_4);
        filesListView->setObjectName("filesListView");
        filesListView->setEnabled(true);
        sizePolicy2.setHeightForWidth(filesListView->sizePolicy().hasHeightForWidth());
        filesListView->setSizePolicy(sizePolicy2);
        filesListView->setMinimumSize(QSize(270, 370));
        filesListView->setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustIgnored);
        filesListView->setTabKeyNavigation(true);
        filesListView->setDragEnabled(false);
        filesListView->setDragDropOverwriteMode(true);
        filesListView->setAlternatingRowColors(true);
        filesListView->setSortingEnabled(true);
        filesListView->horizontalHeader()->setMinimumSectionSize(30);
        filesListView->horizontalHeader()->setDefaultSectionSize(89);
        filesListView->horizontalHeader()->setHighlightSections(false);
        filesListView->horizontalHeader()->setStretchLastSection(true);
        filesListView->verticalHeader()->setCascadingSectionResizes(false);
        filesListView->verticalHeader()->setProperty("showSortIndicator", QVariant(false));
        filesListView->verticalHeader()->setStretchLastSection(false);

        verticalLayout_9->addWidget(filesListView);

        addSpanFilesButton = new QPushButton(verticalLayoutWidget_4);
        addSpanFilesButton->setObjectName("addSpanFilesButton");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(addSpanFilesButton->sizePolicy().hasHeightForWidth());
        addSpanFilesButton->setSizePolicy(sizePolicy3);
        QIcon icon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen));
        addSpanFilesButton->setIcon(icon);

        verticalLayout_9->addWidget(addSpanFilesButton);

        viewButton = new QPushButton(verticalLayoutWidget_4);
        viewButton->setObjectName("viewButton");
        sizePolicy3.setHeightForWidth(viewButton->sizePolicy().hasHeightForWidth());
        viewButton->setSizePolicy(sizePolicy3);

        verticalLayout_9->addWidget(viewButton);

        removeSpanFilesButton = new QPushButton(verticalLayoutWidget_4);
        removeSpanFilesButton->setObjectName("removeSpanFilesButton");
        sizePolicy3.setHeightForWidth(removeSpanFilesButton->sizePolicy().hasHeightForWidth());
        removeSpanFilesButton->setSizePolicy(sizePolicy3);
        QIcon icon1(QIcon::fromTheme(QIcon::ThemeIcon::EditDelete));
        removeSpanFilesButton->setIcon(icon1);

        verticalLayout_9->addWidget(removeSpanFilesButton);

        infoButton = new QPushButton(verticalLayoutWidget_4);
        infoButton->setObjectName("infoButton");
        sizePolicy3.setHeightForWidth(infoButton->sizePolicy().hasHeightForWidth());
        infoButton->setSizePolicy(sizePolicy3);
        QIcon icon2(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout));
        infoButton->setIcon(icon2);

        verticalLayout_9->addWidget(infoButton);

        placeLabelsButton = new QPushButton(verticalLayoutWidget_4);
        placeLabelsButton->setObjectName("placeLabelsButton");
        sizePolicy3.setHeightForWidth(placeLabelsButton->sizePolicy().hasHeightForWidth());
        placeLabelsButton->setSizePolicy(sizePolicy3);

        verticalLayout_9->addWidget(placeLabelsButton);

        preprocessButton = new QPushButton(verticalLayoutWidget_4);
        preprocessButton->setObjectName("preprocessButton");
        sizePolicy3.setHeightForWidth(preprocessButton->sizePolicy().hasHeightForWidth());
        preprocessButton->setSizePolicy(sizePolicy3);
        preprocessButton->setMinimumSize(QSize(0, 0));
        preprocessButton->setMaximumSize(QSize(16777215, 16777215));
        preprocessButton->setAutoDefault(false);

        verticalLayout_9->addWidget(preprocessButton);

        verticalSpacer_3 = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_9->addItem(verticalSpacer_3);

        tabWidget->addTab(filesTab, QString());
        labelTab = new QWidget();
        labelTab->setObjectName("labelTab");
        sizePolicy.setHeightForWidth(labelTab->sizePolicy().hasHeightForWidth());
        labelTab->setSizePolicy(sizePolicy);
        verticalLayoutWidget_6 = new QWidget(labelTab);
        verticalLayoutWidget_6->setObjectName("verticalLayoutWidget_6");
        verticalLayoutWidget_6->setGeometry(QRect(10, 0, 272, 301));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget_6);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        landmarkListView = new QTableView(verticalLayoutWidget_6);
        landmarkListView->setObjectName("landmarkListView");
        QSizePolicy sizePolicy4(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(landmarkListView->sizePolicy().hasHeightForWidth());
        landmarkListView->setSizePolicy(sizePolicy4);
        landmarkListView->setMinimumSize(QSize(270, 210));
        landmarkListView->setMaximumSize(QSize(16777215, 400));
        landmarkListView->setDragEnabled(false);
        landmarkListView->setDragDropOverwriteMode(false);
        landmarkListView->setAlternatingRowColors(true);
        landmarkListView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        landmarkListView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        landmarkListView->setSortingEnabled(true);
        landmarkListView->horizontalHeader()->setVisible(true);
        landmarkListView->horizontalHeader()->setCascadingSectionResizes(false);
        landmarkListView->horizontalHeader()->setMinimumSectionSize(10);
        landmarkListView->horizontalHeader()->setDefaultSectionSize(20);
        landmarkListView->horizontalHeader()->setHighlightSections(false);
        landmarkListView->horizontalHeader()->setProperty("showSortIndicator", QVariant(true));
        landmarkListView->horizontalHeader()->setStretchLastSection(false);
        landmarkListView->verticalHeader()->setCascadingSectionResizes(false);
        landmarkListView->verticalHeader()->setDefaultSectionSize(24);
        landmarkListView->verticalHeader()->setHighlightSections(false);
        landmarkListView->verticalHeader()->setProperty("showSortIndicator", QVariant(false));
        landmarkListView->verticalHeader()->setStretchLastSection(false);

        verticalLayout->addWidget(landmarkListView);

        verticalLayout_11 = new QVBoxLayout();
        verticalLayout_11->setSpacing(0);
        verticalLayout_11->setObjectName("verticalLayout_11");
        getCSVFileButton = new QPushButton(verticalLayoutWidget_6);
        getCSVFileButton->setObjectName("getCSVFileButton");
        sizePolicy3.setHeightForWidth(getCSVFileButton->sizePolicy().hasHeightForWidth());
        getCSVFileButton->setSizePolicy(sizePolicy3);

        verticalLayout_11->addWidget(getCSVFileButton);

        removeLandmarkButton = new QPushButton(verticalLayoutWidget_6);
        removeLandmarkButton->setObjectName("removeLandmarkButton");
        sizePolicy3.setHeightForWidth(removeLandmarkButton->sizePolicy().hasHeightForWidth());
        removeLandmarkButton->setSizePolicy(sizePolicy3);

        verticalLayout_11->addWidget(removeLandmarkButton);

        clearAllLandmarkButton = new QPushButton(verticalLayoutWidget_6);
        clearAllLandmarkButton->setObjectName("clearAllLandmarkButton");
        sizePolicy3.setHeightForWidth(clearAllLandmarkButton->sizePolicy().hasHeightForWidth());
        clearAllLandmarkButton->setSizePolicy(sizePolicy3);

        verticalLayout_11->addWidget(clearAllLandmarkButton);


        verticalLayout->addLayout(verticalLayout_11);

        templateGroupBox = new QGroupBox(labelTab);
        templateGroupBox->setObjectName("templateGroupBox");
        templateGroupBox->setGeometry(QRect(10, 310, 271, 221));
        QFont font1;
        font1.setBold(true);
        templateGroupBox->setFont(font1);
        templateComboBox = new QComboBox(templateGroupBox);
        templateComboBox->setObjectName("templateComboBox");
        templateComboBox->setGeometry(QRect(10, 30, 251, 22));
        QFont font2;
        font2.setBold(false);
        templateComboBox->setFont(font2);
        dtwRateSpinBox = new QSpinBox(templateGroupBox);
        dtwRateSpinBox->setObjectName("dtwRateSpinBox");
        dtwRateSpinBox->setGeometry(QRect(139, 60, 121, 22));
        dtwRateSpinBox->setFont(font2);
        dtwRateSpinBox->setMinimum(0);
        dtwRateSpinBox->setMaximum(1000);
        dtwRateSpinBox->setValue(200);
        dtwMaxLenSpinBox = new QSpinBox(templateGroupBox);
        dtwMaxLenSpinBox->setObjectName("dtwMaxLenSpinBox");
        dtwMaxLenSpinBox->setGeometry(QRect(139, 90, 121, 22));
        dtwMaxLenSpinBox->setFont(font2);
        dtwMaxLenSpinBox->setMaximum(20000);
        dtwMaxLenSpinBox->setValue(5000);
        label_13 = new QLabel(templateGroupBox);
        label_13->setObjectName("label_13");
        label_13->setGeometry(QRect(10, 60, 91, 16));
        label_13->setFont(font2);
        label_14 = new QLabel(templateGroupBox);
        label_14->setObjectName("label_14");
        label_14->setGeometry(QRect(10, 90, 91, 16));
        label_14->setFont(font2);
        verticalLayoutWidget_8 = new QWidget(templateGroupBox);
        verticalLayoutWidget_8->setObjectName("verticalLayoutWidget_8");
        verticalLayoutWidget_8->setGeometry(QRect(10, 150, 129, 71));
        verticalLayout_12 = new QVBoxLayout(verticalLayoutWidget_8);
        verticalLayout_12->setSpacing(0);
        verticalLayout_12->setObjectName("verticalLayout_12");
        verticalLayout_12->setContentsMargins(0, 0, 0, 0);
        saveTemplateButton = new QPushButton(verticalLayoutWidget_8);
        saveTemplateButton->setObjectName("saveTemplateButton");
        sizePolicy3.setHeightForWidth(saveTemplateButton->sizePolicy().hasHeightForWidth());
        saveTemplateButton->setSizePolicy(sizePolicy3);
        saveTemplateButton->setFont(font2);

        verticalLayout_12->addWidget(saveTemplateButton);

        importTemplateButton = new QPushButton(verticalLayoutWidget_8);
        importTemplateButton->setObjectName("importTemplateButton");
        sizePolicy3.setHeightForWidth(importTemplateButton->sizePolicy().hasHeightForWidth());
        importTemplateButton->setSizePolicy(sizePolicy3);
        importTemplateButton->setFont(font2);

        verticalLayout_12->addWidget(importTemplateButton);

        verticalLayoutWidget_9 = new QWidget(templateGroupBox);
        verticalLayoutWidget_9->setObjectName("verticalLayoutWidget_9");
        verticalLayoutWidget_9->setGeometry(QRect(140, 120, 121, 101));
        verticalLayout_13 = new QVBoxLayout(verticalLayoutWidget_9);
        verticalLayout_13->setSpacing(0);
        verticalLayout_13->setObjectName("verticalLayout_13");
        verticalLayout_13->setContentsMargins(0, 0, 0, 0);
        dtwDefaultsButton = new QPushButton(verticalLayoutWidget_9);
        dtwDefaultsButton->setObjectName("dtwDefaultsButton");
        sizePolicy3.setHeightForWidth(dtwDefaultsButton->sizePolicy().hasHeightForWidth());
        dtwDefaultsButton->setSizePolicy(sizePolicy3);
        dtwDefaultsButton->setFont(font2);

        verticalLayout_13->addWidget(dtwDefaultsButton);

        applyTemplateButton = new QPushButton(verticalLayoutWidget_9);
        applyTemplateButton->setObjectName("applyTemplateButton");
        sizePolicy3.setHeightForWidth(applyTemplateButton->sizePolicy().hasHeightForWidth());
        applyTemplateButton->setSizePolicy(sizePolicy3);
        applyTemplateButton->setFont(font2);

        verticalLayout_13->addWidget(applyTemplateButton);

        batchApplyTemplateButton = new QPushButton(verticalLayoutWidget_9);
        batchApplyTemplateButton->setObjectName("batchApplyTemplateButton");
        sizePolicy3.setHeightForWidth(batchApplyTemplateButton->sizePolicy().hasHeightForWidth());
        batchApplyTemplateButton->setSizePolicy(sizePolicy3);
        batchApplyTemplateButton->setFont(font2);

        verticalLayout_13->addWidget(batchApplyTemplateButton);

        tabWidget->addTab(labelTab, QString());
        configureTab = new QWidget();
        configureTab->setObjectName("configureTab");
        QSizePolicy sizePolicy5(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(configureTab->sizePolicy().hasHeightForWidth());
        configureTab->setSizePolicy(sizePolicy5);
        configureTab->setMinimumSize(QSize(0, 20));
        ch1ComboBox = new QComboBox(configureTab);
        ch1ComboBox->setObjectName("ch1ComboBox");
        ch1ComboBox->setGeometry(QRect(70, 390, 80, 24));
        QSizePolicy sizePolicy6(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(ch1ComboBox->sizePolicy().hasHeightForWidth());
        ch1ComboBox->setSizePolicy(sizePolicy6);
        ch2ComboBox = new QComboBox(configureTab);
        ch2ComboBox->setObjectName("ch2ComboBox");
        ch2ComboBox->setGeometry(QRect(190, 390, 80, 24));
        sizePolicy6.setHeightForWidth(ch2ComboBox->sizePolicy().hasHeightForWidth());
        ch2ComboBox->setSizePolicy(sizePolicy6);
        label_9 = new QLabel(configureTab);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(30, 390, 41, 20));
        label_9->setAlignment(Qt::AlignmentFlag::AlignCenter);
        label_10 = new QLabel(configureTab);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(150, 390, 41, 20));
        label_10->setAlignment(Qt::AlignmentFlag::AlignCenter);
        derivedChLineEdit = new QLineEdit(configureTab);
        derivedChLineEdit->setObjectName("derivedChLineEdit");
        derivedChLineEdit->setGeometry(QRect(80, 420, 191, 24));
        sizePolicy6.setHeightForWidth(derivedChLineEdit->sizePolicy().hasHeightForWidth());
        derivedChLineEdit->setSizePolicy(sizePolicy6);
        label_11 = new QLabel(configureTab);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(20, 420, 41, 21));
        channelTypeComboBox = new QComboBox(configureTab);
        channelTypeComboBox->setObjectName("channelTypeComboBox");
        channelTypeComboBox->setGeometry(QRect(70, 450, 201, 24));
        sizePolicy6.setHeightForWidth(channelTypeComboBox->sizePolicy().hasHeightForWidth());
        channelTypeComboBox->setSizePolicy(sizePolicy6);
        horizontalLayoutWidget = new QWidget(configureTab);
        horizontalLayoutWidget->setObjectName("horizontalLayoutWidget");
        horizontalLayoutWidget->setGeometry(QRect(19, 480, 251, 32));
        horizontalLayout_6 = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout_6->setSpacing(0);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        clearAllDerivedButton = new QPushButton(horizontalLayoutWidget);
        clearAllDerivedButton->setObjectName("clearAllDerivedButton");
        sizePolicy3.setHeightForWidth(clearAllDerivedButton->sizePolicy().hasHeightForWidth());
        clearAllDerivedButton->setSizePolicy(sizePolicy3);

        horizontalLayout_6->addWidget(clearAllDerivedButton);

        deriveButton = new QPushButton(horizontalLayoutWidget);
        deriveButton->setObjectName("deriveButton");
        sizePolicy3.setHeightForWidth(deriveButton->sizePolicy().hasHeightForWidth());
        deriveButton->setSizePolicy(sizePolicy3);

        horizontalLayout_6->addWidget(deriveButton);

        label_12 = new QLabel(configureTab);
        label_12->setObjectName("label_12");
        label_12->setGeometry(QRect(20, 450, 51, 21));
        groupBox_2 = new QGroupBox(configureTab);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(10, 0, 271, 181));
        groupBox_2->setFont(font1);
        channelsComboBox = new QComboBox(groupBox_2);
        channelsComboBox->setObjectName("channelsComboBox");
        channelsComboBox->setGeometry(QRect(10, 30, 251, 20));
        sizePolicy6.setHeightForWidth(channelsComboBox->sizePolicy().hasHeightForWidth());
        channelsComboBox->setSizePolicy(sizePolicy6);
        channelsComboBox->setFont(font2);
        X_checkBox = new QCheckBox(groupBox_2);
        X_checkBox->setObjectName("X_checkBox");
        X_checkBox->setGeometry(QRect(20, 70, 101, 20));
        X_checkBox->setFont(font2);
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(140, 50, 111, 20));
        label_3->setFont(font1);
        Y_checkBox = new QCheckBox(groupBox_2);
        Y_checkBox->setObjectName("Y_checkBox");
        Y_checkBox->setGeometry(QRect(20, 90, 101, 20));
        Y_checkBox->setFont(font2);
        Z_checkBox = new QCheckBox(groupBox_2);
        Z_checkBox->setObjectName("Z_checkBox");
        Z_checkBox->setGeometry(QRect(20, 110, 101, 20));
        Z_checkBox->setFont(font2);
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(20, 50, 101, 20));
        label_2->setFont(font1);
        displacementRB = new QRadioButton(groupBox_2);
        displacementRB->setObjectName("displacementRB");
        displacementRB->setGeometry(QRect(140, 70, 121, 20));
        displacementRB->setFont(font2);
        velocityRB = new QRadioButton(groupBox_2);
        velocityRB->setObjectName("velocityRB");
        velocityRB->setGeometry(QRect(140, 90, 121, 20));
        velocityRB->setFont(font2);
        accelerationRB = new QRadioButton(groupBox_2);
        accelerationRB->setObjectName("accelerationRB");
        accelerationRB->setGeometry(QRect(140, 110, 121, 20));
        accelerationRB->setFont(font2);
        horizontalLayoutWidget_3 = new QWidget(groupBox_2);
        horizontalLayoutWidget_3->setObjectName("horizontalLayoutWidget_3");
        horizontalLayoutWidget_3->setGeometry(QRect(10, 131, 251, 41));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget_3);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setSizeConstraint(QLayout::SizeConstraint::SetNoConstraint);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        addConfigButton = new QPushButton(horizontalLayoutWidget_3);
        addConfigButton->setObjectName("addConfigButton");
        sizePolicy3.setHeightForWidth(addConfigButton->sizePolicy().hasHeightForWidth());
        addConfigButton->setSizePolicy(sizePolicy3);
        QFont font3;
        font3.setBold(false);
        font3.setUnderline(false);
        font3.setStrikeOut(false);
        font3.setKerning(true);
        addConfigButton->setFont(font3);
        addConfigButton->setIconSize(QSize(16, 16));

        horizontalLayout->addWidget(addConfigButton);

        updateConfigButton = new QPushButton(horizontalLayoutWidget_3);
        updateConfigButton->setObjectName("updateConfigButton");
        updateConfigButton->setEnabled(true);
        sizePolicy3.setHeightForWidth(updateConfigButton->sizePolicy().hasHeightForWidth());
        updateConfigButton->setSizePolicy(sizePolicy3);
        updateConfigButton->setFont(font2);

        horizontalLayout->addWidget(updateConfigButton);

        groupBox_5 = new QGroupBox(configureTab);
        groupBox_5->setObjectName("groupBox_5");
        groupBox_5->setGeometry(QRect(10, 180, 271, 191));
        groupBox_5->setFont(font1);
        horizontalLayoutWidget_4 = new QWidget(groupBox_5);
        horizontalLayoutWidget_4->setObjectName("horizontalLayoutWidget_4");
        horizontalLayoutWidget_4->setGeometry(QRect(10, 150, 251, 41));
        horizontalLayout_3 = new QHBoxLayout(horizontalLayoutWidget_4);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        configureButton = new QPushButton(horizontalLayoutWidget_4);
        configureButton->setObjectName("configureButton");
        sizePolicy3.setHeightForWidth(configureButton->sizePolicy().hasHeightForWidth());
        configureButton->setSizePolicy(sizePolicy3);
        configureButton->setFont(font2);

        horizontalLayout_3->addWidget(configureButton);

        applyConfigToAllButton = new QPushButton(horizontalLayoutWidget_4);
        applyConfigToAllButton->setObjectName("applyConfigToAllButton");
        sizePolicy3.setHeightForWidth(applyConfigToAllButton->sizePolicy().hasHeightForWidth());
        applyConfigToAllButton->setSizePolicy(sizePolicy3);
        applyConfigToAllButton->setFont(font2);

        horizontalLayout_3->addWidget(applyConfigToAllButton);

        verticalLayoutWidget_5 = new QWidget(groupBox_5);
        verticalLayoutWidget_5->setObjectName("verticalLayoutWidget_5");
        verticalLayoutWidget_5->setGeometry(QRect(140, 20, 121, 131));
        verticalLayout_10 = new QVBoxLayout(verticalLayoutWidget_5);
        verticalLayout_10->setSpacing(0);
        verticalLayout_10->setObjectName("verticalLayout_10");
        verticalLayout_10->setContentsMargins(0, 0, 0, 0);
        moveUpButton = new QPushButton(verticalLayoutWidget_5);
        moveUpButton->setObjectName("moveUpButton");
        sizePolicy3.setHeightForWidth(moveUpButton->sizePolicy().hasHeightForWidth());
        moveUpButton->setSizePolicy(sizePolicy3);
        moveUpButton->setFont(font2);

        verticalLayout_10->addWidget(moveUpButton);

        moveDownButton = new QPushButton(verticalLayoutWidget_5);
        moveDownButton->setObjectName("moveDownButton");
        sizePolicy3.setHeightForWidth(moveDownButton->sizePolicy().hasHeightForWidth());
        moveDownButton->setSizePolicy(sizePolicy3);
        moveDownButton->setFont(font2);

        verticalLayout_10->addWidget(moveDownButton);

        invertButton = new QPushButton(verticalLayoutWidget_5);
        invertButton->setObjectName("invertButton");
        sizePolicy3.setHeightForWidth(invertButton->sizePolicy().hasHeightForWidth());
        invertButton->setSizePolicy(sizePolicy3);
        invertButton->setFont(font2);

        verticalLayout_10->addWidget(invertButton);

        removeConfigButton = new QPushButton(verticalLayoutWidget_5);
        removeConfigButton->setObjectName("removeConfigButton");
        sizePolicy3.setHeightForWidth(removeConfigButton->sizePolicy().hasHeightForWidth());
        removeConfigButton->setSizePolicy(sizePolicy3);
        removeConfigButton->setFont(font2);

        verticalLayout_10->addWidget(removeConfigButton);

        configuredChannelsListView = new QListView(configureTab);
        configuredChannelsListView->setObjectName("configuredChannelsListView");
        configuredChannelsListView->setGeometry(QRect(20, 210, 121, 111));
        sizePolicy2.setHeightForWidth(configuredChannelsListView->sizePolicy().hasHeightForWidth());
        configuredChannelsListView->setSizePolicy(sizePolicy2);
        configuredChannelsListView->setFont(font2);
        groupBox_6 = new QGroupBox(configureTab);
        groupBox_6->setObjectName("groupBox_6");
        groupBox_6->setGeometry(QRect(10, 370, 271, 151));
        groupBox_6->setFont(font1);
        tabWidget->addTab(configureTab, QString());
        groupBox_6->raise();
        ch1ComboBox->raise();
        ch2ComboBox->raise();
        label_9->raise();
        label_10->raise();
        derivedChLineEdit->raise();
        label_11->raise();
        channelTypeComboBox->raise();
        horizontalLayoutWidget->raise();
        label_12->raise();
        groupBox_2->raise();
        groupBox_5->raise();
        configuredChannelsListView->raise();
        settingsTab = new QWidget();
        settingsTab->setObjectName("settingsTab");
        sizePolicy.setHeightForWidth(settingsTab->sizePolicy().hasHeightForWidth());
        settingsTab->setSizePolicy(sizePolicy);
        groupBox_7 = new QGroupBox(settingsTab);
        groupBox_7->setObjectName("groupBox_7");
        groupBox_7->setGeometry(QRect(10, 10, 271, 161));
        groupBox_7->setFont(font1);
        label = new QLabel(groupBox_7);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 21, 101, 20));
        label->setFont(font2);
        label_4 = new QLabel(groupBox_7);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(10, 50, 121, 21));
        label_4->setFont(font2);
        label_5 = new QLabel(groupBox_7);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(10, 80, 111, 21));
        label_5->setFont(font2);
        horizontalLayoutWidget_2 = new QWidget(groupBox_7);
        horizontalLayoutWidget_2->setObjectName("horizontalLayoutWidget_2");
        horizontalLayoutWidget_2->setGeometry(QRect(9, 109, 251, 32));
        horizontalLayout_5 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        specDefaultsButton = new QPushButton(horizontalLayoutWidget_2);
        specDefaultsButton->setObjectName("specDefaultsButton");
        specDefaultsButton->setFont(font2);

        horizontalLayout_5->addWidget(specDefaultsButton);

        specApplyButton = new QPushButton(horizontalLayoutWidget_2);
        specApplyButton->setObjectName("specApplyButton");
        specApplyButton->setFont(font2);

        horizontalLayout_5->addWidget(specApplyButton);

        verticalLayoutWidget_10 = new QWidget(groupBox_7);
        verticalLayoutWidget_10->setObjectName("verticalLayoutWidget_10");
        verticalLayoutWidget_10->setGeometry(QRect(133, 24, 131, 85));
        verticalLayout_14 = new QVBoxLayout(verticalLayoutWidget_10);
        verticalLayout_14->setObjectName("verticalLayout_14");
        verticalLayout_14->setContentsMargins(0, 0, 0, 0);
        specViewMaxHzSpinBox = new QDoubleSpinBox(verticalLayoutWidget_10);
        specViewMaxHzSpinBox->setObjectName("specViewMaxHzSpinBox");
        specViewMaxHzSpinBox->setFont(font2);
        specViewMaxHzSpinBox->setMinimum(500.000000000000000);
        specViewMaxHzSpinBox->setMaximum(8000.000000000000000);

        verticalLayout_14->addWidget(specViewMaxHzSpinBox);

        specWindowMsSpinBox = new QDoubleSpinBox(verticalLayoutWidget_10);
        specWindowMsSpinBox->setObjectName("specWindowMsSpinBox");
        specWindowMsSpinBox->setFont(font2);
        specWindowMsSpinBox->setMinimum(5.000000000000000);
        specWindowMsSpinBox->setMaximum(200.000000000000000);
        specWindowMsSpinBox->setSingleStep(5.000000000000000);

        verticalLayout_14->addWidget(specWindowMsSpinBox);

        specDynamicRangeSpinBox = new QDoubleSpinBox(verticalLayoutWidget_10);
        specDynamicRangeSpinBox->setObjectName("specDynamicRangeSpinBox");
        specDynamicRangeSpinBox->setFont(font2);
        specDynamicRangeSpinBox->setMinimum(10.000000000000000);
        specDynamicRangeSpinBox->setMaximum(120.000000000000000);

        verticalLayout_14->addWidget(specDynamicRangeSpinBox);

        verticalLayoutWidget_2 = new QWidget(settingsTab);
        verticalLayoutWidget_2->setObjectName("verticalLayoutWidget_2");
        verticalLayoutWidget_2->setGeometry(QRect(10, 300, 271, 221));
        verticalLayout_5 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_5->setObjectName("verticalLayout_5");
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        hl3DMAIN = new QVBoxLayout();
        hl3DMAIN->setSpacing(0);
        hl3DMAIN->setObjectName("hl3DMAIN");
        hl3DMAIN->setSizeConstraint(QLayout::SizeConstraint::SetNoConstraint);

        verticalLayout_5->addLayout(hl3DMAIN);

        tabWidget->addTab(settingsTab, QString());

        verticalLayout_7->addWidget(tabWidget);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        verticalLayout_2->setContentsMargins(-1, -1, -1, 0);
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        sizePolicy6.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy6);
        groupBox->setMinimumSize(QSize(300, 60));
        groupBox->setFont(font1);
        horizontalLayoutWidget_6 = new QWidget(groupBox);
        horizontalLayoutWidget_6->setObjectName("horizontalLayoutWidget_6");
        horizontalLayoutWidget_6->setGeometry(QRect(10, 20, 281, 41));
        horizontalLayout_10 = new QHBoxLayout(horizontalLayoutWidget_6);
        horizontalLayout_10->setSpacing(0);
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        horizontalLayout_10->setContentsMargins(0, 0, 0, 0);
        playSoundButton = new QPushButton(horizontalLayoutWidget_6);
        playSoundButton->setObjectName("playSoundButton");
        sizePolicy3.setHeightForWidth(playSoundButton->sizePolicy().hasHeightForWidth());
        playSoundButton->setSizePolicy(sizePolicy3);
        playSoundButton->setFont(font2);
        QIcon icon3(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStart));
        playSoundButton->setIcon(icon3);

        horizontalLayout_10->addWidget(playSoundButton);

        playSelectionButton = new QPushButton(horizontalLayoutWidget_6);
        playSelectionButton->setObjectName("playSelectionButton");
        playSelectionButton->setFont(font2);
        playSelectionButton->setIcon(icon3);

        horizontalLayout_10->addWidget(playSelectionButton);

        stopSoundButton = new QPushButton(horizontalLayoutWidget_6);
        stopSoundButton->setObjectName("stopSoundButton");
        stopSoundButton->setFont(font2);
        QIcon icon4(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStop));
        stopSoundButton->setIcon(icon4);

        horizontalLayout_10->addWidget(stopSoundButton);


        verticalLayout_2->addWidget(groupBox);

        groupBox_3 = new QGroupBox(centralwidget);
        groupBox_3->setObjectName("groupBox_3");
        sizePolicy6.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy6);
        groupBox_3->setMinimumSize(QSize(300, 60));
        groupBox_3->setFont(font1);
        layoutWidget = new QWidget(groupBox_3);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 20, 281, 41));
        horizontalLayout_4 = new QHBoxLayout(layoutWidget);
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        zoomInButton = new QPushButton(layoutWidget);
        zoomInButton->setObjectName("zoomInButton");
        zoomInButton->setFont(font2);
        zoomInButton->setText(QString::fromUtf8("In"));
        QIcon icon5(QIcon::fromTheme(QIcon::ThemeIcon::ZoomIn));
        zoomInButton->setIcon(icon5);

        horizontalLayout_4->addWidget(zoomInButton);

        zoomOutButton = new QPushButton(layoutWidget);
        zoomOutButton->setObjectName("zoomOutButton");
        QSizePolicy sizePolicy7(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(zoomOutButton->sizePolicy().hasHeightForWidth());
        zoomOutButton->setSizePolicy(sizePolicy7);
        zoomOutButton->setFont(font2);
        QIcon icon6(QIcon::fromTheme(QIcon::ThemeIcon::ZoomOut));
        zoomOutButton->setIcon(icon6);

        horizontalLayout_4->addWidget(zoomOutButton);

        resetButton = new QPushButton(layoutWidget);
        resetButton->setObjectName("resetButton");
        resetButton->setFont(font2);
        QIcon icon7(QIcon::fromTheme(QIcon::ThemeIcon::ViewRestore));
        resetButton->setIcon(icon7);

        horizontalLayout_4->addWidget(resetButton);

        selectButton = new QPushButton(layoutWidget);
        selectButton->setObjectName("selectButton");
        selectButton->setFont(font2);
        QIcon icon8(QIcon::fromTheme(QIcon::ThemeIcon::ZoomFitBest));
        selectButton->setIcon(icon8);

        horizontalLayout_4->addWidget(selectButton);


        verticalLayout_2->addWidget(groupBox_3);

        groupBox_4 = new QGroupBox(centralwidget);
        groupBox_4->setObjectName("groupBox_4");
        sizePolicy6.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy6);
        groupBox_4->setMinimumSize(QSize(300, 50));
        groupBox_4->setFont(font1);
        layoutWidget1 = new QWidget(groupBox_4);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(10, 20, 181, 31));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget1);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setSizeConstraint(QLayout::SizeConstraint::SetNoConstraint);
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        trackXRadioButton = new QRadioButton(layoutWidget1);
        trackXRadioButton->setObjectName("trackXRadioButton");
        sizePolicy6.setHeightForWidth(trackXRadioButton->sizePolicy().hasHeightForWidth());
        trackXRadioButton->setSizePolicy(sizePolicy6);
        trackXRadioButton->setFont(font2);

        horizontalLayout_2->addWidget(trackXRadioButton);

        trackYRadioButton = new QRadioButton(layoutWidget1);
        trackYRadioButton->setObjectName("trackYRadioButton");
        sizePolicy6.setHeightForWidth(trackYRadioButton->sizePolicy().hasHeightForWidth());
        trackYRadioButton->setSizePolicy(sizePolicy6);
        trackYRadioButton->setFont(font2);

        horizontalLayout_2->addWidget(trackYRadioButton);

        trackZRadioButton = new QRadioButton(layoutWidget1);
        trackZRadioButton->setObjectName("trackZRadioButton");
        sizePolicy6.setHeightForWidth(trackZRadioButton->sizePolicy().hasHeightForWidth());
        trackZRadioButton->setSizePolicy(sizePolicy6);
        trackZRadioButton->setFont(font2);

        horizontalLayout_2->addWidget(trackZRadioButton);

        trackAutoRadioButton = new QRadioButton(layoutWidget1);
        trackAutoRadioButton->setObjectName("trackAutoRadioButton");
        sizePolicy6.setHeightForWidth(trackAutoRadioButton->sizePolicy().hasHeightForWidth());
        trackAutoRadioButton->setSizePolicy(sizePolicy6);
        trackAutoRadioButton->setFont(font2);

        horizontalLayout_2->addWidget(trackAutoRadioButton);


        verticalLayout_2->addWidget(groupBox_4);


        verticalLayout_7->addLayout(verticalLayout_2);


        gridLayout->addLayout(verticalLayout_7, 3, 0, 1, 1);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(3);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setSizeConstraint(QLayout::SizeConstraint::SetNoConstraint);
        fileNameLabel = new QLabel(centralwidget);
        fileNameLabel->setObjectName("fileNameLabel");
        sizePolicy5.setHeightForWidth(fileNameLabel->sizePolicy().hasHeightForWidth());
        fileNameLabel->setSizePolicy(sizePolicy5);
        QFont font4;
        font4.setBold(true);
        font4.setUnderline(false);
        fileNameLabel->setFont(font4);
        fileNameLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_3->addWidget(fileNameLabel);

        audioContainer = new QWidget(centralwidget);
        audioContainer->setObjectName("audioContainer");
        sizePolicy1.setHeightForWidth(audioContainer->sizePolicy().hasHeightForWidth());
        audioContainer->setSizePolicy(sizePolicy1);
        audioContainer->setMinimumSize(QSize(0, 0));
        verticalLayout_4 = new QVBoxLayout(audioContainer);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setSizeConstraint(QLayout::SizeConstraint::SetNoConstraint);
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        audioPlotVL = new QVBoxLayout();
        audioPlotVL->setSpacing(0);
        audioPlotVL->setObjectName("audioPlotVL");
        audioPlotVL->setSizeConstraint(QLayout::SizeConstraint::SetNoConstraint);
        audioPlotVL->setContentsMargins(-1, -1, 0, -1);

        verticalLayout_4->addLayout(audioPlotVL);


        verticalLayout_3->addWidget(audioContainer, 0, Qt::AlignmentFlag::AlignTop);

        scrollArea = new QScrollArea(centralwidget);
        scrollArea->setObjectName("scrollArea");
        sizePolicy1.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy1);
        scrollArea->setToolTipDuration(0);
        scrollArea->setFrameShape(QFrame::Shape::NoFrame);
        scrollArea->setFrameShadow(QFrame::Shadow::Plain);
        scrollArea->setLineWidth(0);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustIgnored);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 656, 684));
        sizePolicy.setHeightForWidth(scrollAreaWidgetContents->sizePolicy().hasHeightForWidth());
        scrollAreaWidgetContents->setSizePolicy(sizePolicy);
        scrollAreaWidgetContents->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        verticalLayout_8 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_8->setSpacing(0);
        verticalLayout_8->setObjectName("verticalLayout_8");
        verticalLayout_8->setSizeConstraint(QLayout::SizeConstraint::SetNoConstraint);
        verticalLayout_8->setContentsMargins(0, 0, 0, 0);
        posPlotVL = new QVBoxLayout();
        posPlotVL->setSpacing(0);
        posPlotVL->setObjectName("posPlotVL");
        posPlotVL->setSizeConstraint(QLayout::SizeConstraint::SetNoConstraint);
        posPlotVL->setContentsMargins(-1, 0, -1, 0);

        verticalLayout_8->addLayout(posPlotVL);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout_3->addWidget(scrollArea);

        rangeScrollBar = new QScrollBar(centralwidget);
        rangeScrollBar->setObjectName("rangeScrollBar");
        QSizePolicy sizePolicy8(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Minimum);
        sizePolicy8.setHorizontalStretch(0);
        sizePolicy8.setVerticalStretch(0);
        sizePolicy8.setHeightForWidth(rangeScrollBar->sizePolicy().hasHeightForWidth());
        rangeScrollBar->setSizePolicy(sizePolicy8);
        rangeScrollBar->setOrientation(Qt::Orientation::Horizontal);

        verticalLayout_3->addWidget(rangeScrollBar, 0, Qt::AlignmentFlag::AlignBottom);


        gridLayout->addLayout(verticalLayout_3, 3, 1, 1, 3);

        mainWindow->setCentralWidget(centralwidget);

        retranslateUi(mainWindow);

        tabWidget->setCurrentIndex(0);
        preprocessButton->setDefault(false);


        QMetaObject::connectSlotsByName(mainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *mainWindow)
    {
        mainWindow->setWindowTitle(QCoreApplication::translate("mainWindow", "SPAN", nullptr));
        actionOpen->setText(QCoreApplication::translate("mainWindow", "Open...", nullptr));
        actionOpen_Recent->setText(QCoreApplication::translate("mainWindow", "Open Recent", nullptr));
        addSpanFilesButton->setText(QCoreApplication::translate("mainWindow", "Open", nullptr));
        viewButton->setText(QCoreApplication::translate("mainWindow", "View", nullptr));
        removeSpanFilesButton->setText(QCoreApplication::translate("mainWindow", "Remove", nullptr));
        infoButton->setText(QCoreApplication::translate("mainWindow", "Info", nullptr));
        placeLabelsButton->setText(QCoreApplication::translate("mainWindow", "Place Labels", nullptr));
        preprocessButton->setText(QCoreApplication::translate("mainWindow", "Pre-Process", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(filesTab), QCoreApplication::translate("mainWindow", "Files", nullptr));
        getCSVFileButton->setText(QCoreApplication::translate("mainWindow", "Export Landmarks", nullptr));
        removeLandmarkButton->setText(QCoreApplication::translate("mainWindow", "Remove", nullptr));
        clearAllLandmarkButton->setText(QCoreApplication::translate("mainWindow", "Clear All", nullptr));
        templateGroupBox->setTitle(QCoreApplication::translate("mainWindow", "DTW Annotation", nullptr));
        label_13->setText(QCoreApplication::translate("mainWindow", "DTW rate (Hz):", nullptr));
        label_14->setText(QCoreApplication::translate("mainWindow", "Max DTW length:", nullptr));
        saveTemplateButton->setText(QCoreApplication::translate("mainWindow", "Save template", nullptr));
        importTemplateButton->setText(QCoreApplication::translate("mainWindow", "Import template", nullptr));
        dtwDefaultsButton->setText(QCoreApplication::translate("mainWindow", "Reset", nullptr));
        applyTemplateButton->setText(QCoreApplication::translate("mainWindow", "Apply", nullptr));
        batchApplyTemplateButton->setText(QCoreApplication::translate("mainWindow", "Apply to ...", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(labelTab), QCoreApplication::translate("mainWindow", "Annotate", nullptr));
        label_9->setText(QCoreApplication::translate("mainWindow", "CH1:", nullptr));
        label_10->setText(QCoreApplication::translate("mainWindow", "CH2:", nullptr));
        label_11->setText(QCoreApplication::translate("mainWindow", "Name", nullptr));
        clearAllDerivedButton->setText(QCoreApplication::translate("mainWindow", "Clear ", nullptr));
        deriveButton->setText(QCoreApplication::translate("mainWindow", "Derive", nullptr));
        label_12->setText(QCoreApplication::translate("mainWindow", "Method", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("mainWindow", "Select channels", nullptr));
        X_checkBox->setText(QCoreApplication::translate("mainWindow", "x", nullptr));
        label_3->setText(QCoreApplication::translate("mainWindow", "Parameters:", nullptr));
        Y_checkBox->setText(QCoreApplication::translate("mainWindow", "y", nullptr));
        Z_checkBox->setText(QCoreApplication::translate("mainWindow", "z", nullptr));
        label_2->setText(QCoreApplication::translate("mainWindow", "Dimensions:", nullptr));
        displacementRB->setText(QCoreApplication::translate("mainWindow", "Displacement", nullptr));
        velocityRB->setText(QCoreApplication::translate("mainWindow", "Velocity", nullptr));
        accelerationRB->setText(QCoreApplication::translate("mainWindow", "Acceleration", nullptr));
        addConfigButton->setText(QCoreApplication::translate("mainWindow", "Add", nullptr));
        updateConfigButton->setText(QCoreApplication::translate("mainWindow", "Update", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("mainWindow", "Configured channels", nullptr));
        configureButton->setText(QCoreApplication::translate("mainWindow", "Configure", nullptr));
        applyConfigToAllButton->setText(QCoreApplication::translate("mainWindow", "Apply to All", nullptr));
        moveUpButton->setText(QCoreApplication::translate("mainWindow", "Up", nullptr));
        moveDownButton->setText(QCoreApplication::translate("mainWindow", "Down", nullptr));
        invertButton->setText(QCoreApplication::translate("mainWindow", "Invert", nullptr));
        removeConfigButton->setText(QCoreApplication::translate("mainWindow", "Remove", nullptr));
        groupBox_6->setTitle(QCoreApplication::translate("mainWindow", "Derive a channel", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(configureTab), QCoreApplication::translate("mainWindow", "Configure", nullptr));
        groupBox_7->setTitle(QCoreApplication::translate("mainWindow", "Audio", nullptr));
        label->setText(QCoreApplication::translate("mainWindow", "Max frequency", nullptr));
        label_4->setText(QCoreApplication::translate("mainWindow", "Window length", nullptr));
        label_5->setText(QCoreApplication::translate("mainWindow", "Dynamic range", nullptr));
        specDefaultsButton->setText(QCoreApplication::translate("mainWindow", "Reset", nullptr));
        specApplyButton->setText(QCoreApplication::translate("mainWindow", "Apply", nullptr));
        specViewMaxHzSpinBox->setSuffix(QCoreApplication::translate("mainWindow", " Hz", nullptr));
        specWindowMsSpinBox->setSuffix(QCoreApplication::translate("mainWindow", " ms", nullptr));
        specDynamicRangeSpinBox->setSuffix(QCoreApplication::translate("mainWindow", " dB", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(settingsTab), QCoreApplication::translate("mainWindow", "Settings", nullptr));
        groupBox->setTitle(QCoreApplication::translate("mainWindow", "Playback", nullptr));
        playSoundButton->setText(QCoreApplication::translate("mainWindow", "Play", nullptr));
        playSelectionButton->setText(QCoreApplication::translate("mainWindow", "Play Select", nullptr));
        stopSoundButton->setText(QCoreApplication::translate("mainWindow", "Stop", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("mainWindow", "VIew", nullptr));
        zoomOutButton->setText(QCoreApplication::translate("mainWindow", "Out", nullptr));
        resetButton->setText(QCoreApplication::translate("mainWindow", "Reset", nullptr));
        selectButton->setText(QCoreApplication::translate("mainWindow", "Select", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("mainWindow", "Tracking", nullptr));
        trackXRadioButton->setText(QCoreApplication::translate("mainWindow", "x", nullptr));
        trackYRadioButton->setText(QCoreApplication::translate("mainWindow", "y", nullptr));
        trackZRadioButton->setText(QCoreApplication::translate("mainWindow", "z", nullptr));
        trackAutoRadioButton->setText(QCoreApplication::translate("mainWindow", "Auto", nullptr));
        fileNameLabel->setText(QCoreApplication::translate("mainWindow", "fileName", nullptr));
    } // retranslateUi

};

namespace Ui {
    class mainWindow: public Ui_mainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

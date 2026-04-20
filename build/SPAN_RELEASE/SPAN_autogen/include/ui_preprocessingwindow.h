/********************************************************************************
** Form generated from reading UI file 'preprocessingWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREPROCESSINGWINDOW_H
#define UI_PREPROCESSINGWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_preprocessingWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_7;
    QGroupBox *groupBox_5;
    QListView *BPFileSelectListView;
    QListView *TrialFileSelectListView;
    QWidget *verticalLayoutWidget_3;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton2;
    QPushButton *pushButton3;
    QPushButton *removeButton;
    QGroupBox *groupBox_4;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *hl3D;
    QWidget *plotLegendWidget;
    QLabel *plotTitleWidget;
    QFrame *line;
    QVBoxLayout *verticalLayout_4;
    QGroupBox *groupBox_3;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout_12;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *hL01;
    QCheckBox *checkBox1;
    QSpacerItem *horizontalSpacer1;
    QComboBox *comboBox1;
    QHBoxLayout *hL09;
    QCheckBox *checkBox9;
    QSpacerItem *horizontalSpacer9;
    QComboBox *comboBox9;
    QHBoxLayout *horizontalLayout_7;
    QHBoxLayout *hL02;
    QCheckBox *checkBox2;
    QSpacerItem *horizontalSpacer2;
    QComboBox *comboBox2;
    QHBoxLayout *hL10;
    QCheckBox *checkBox10;
    QSpacerItem *horizontalSpacer10;
    QComboBox *comboBox10;
    QHBoxLayout *horizontalLayout_9;
    QHBoxLayout *hL03;
    QCheckBox *checkBox3;
    QSpacerItem *horizontalSpacer3;
    QComboBox *comboBox3;
    QHBoxLayout *hL11;
    QCheckBox *checkBox11;
    QSpacerItem *horizontalSpacer11;
    QComboBox *comboBox11;
    QHBoxLayout *horizontalLayout_10;
    QHBoxLayout *hL04;
    QCheckBox *checkBox4;
    QSpacerItem *horizontalSpacer4;
    QComboBox *comboBox4;
    QHBoxLayout *hL12;
    QCheckBox *checkBox12;
    QSpacerItem *horizontalSpacer12;
    QComboBox *comboBox12;
    QHBoxLayout *horizontalLayout_11;
    QHBoxLayout *hL05;
    QCheckBox *checkBox5;
    QSpacerItem *horizontalSpacer5;
    QComboBox *comboBox5;
    QHBoxLayout *hL13;
    QCheckBox *checkBox13;
    QSpacerItem *horizontalSpacer13;
    QComboBox *comboBox13;
    QHBoxLayout *horizontalLayout_12;
    QHBoxLayout *hL06;
    QCheckBox *checkBox6;
    QSpacerItem *horizontalSpacer6;
    QComboBox *comboBox6;
    QHBoxLayout *hL14;
    QCheckBox *checkBox14;
    QSpacerItem *horizontalSpacer14;
    QComboBox *comboBox14;
    QHBoxLayout *horizontalLayout_13;
    QHBoxLayout *hL07;
    QCheckBox *checkBox7;
    QSpacerItem *horizontalSpacer7;
    QComboBox *comboBox7;
    QHBoxLayout *hL15;
    QCheckBox *checkBox15;
    QSpacerItem *horizontalSpacer15;
    QComboBox *comboBox15;
    QHBoxLayout *horizontalLayout_14;
    QHBoxLayout *hL08;
    QCheckBox *checkBox8;
    QSpacerItem *horizontalSpacer8;
    QComboBox *comboBox8;
    QHBoxLayout *hL16;
    QCheckBox *checkBox16;
    QSpacerItem *horizontalSpacer16;
    QComboBox *comboBox16;
    QGroupBox *groupBox;
    QLabel *label_13;
    QComboBox *bpLeftCombo;
    QLabel *label_14;
    QComboBox *bpRightCombo;
    QLabel *label_15;
    QComboBox *bpFrontCombo;
    QGroupBox *groupBox_2;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer;
    QPushButton *startButton;
    QPushButton *proceedButton;
    QPushButton *backButton;
    QPushButton *backButton_2;
    QPushButton *cancelButton;
    QListView *messagesListView;

    void setupUi(QWidget *preprocessingWindow)
    {
        if (preprocessingWindow->objectName().isEmpty())
            preprocessingWindow->setObjectName("preprocessingWindow");
        preprocessingWindow->resize(950, 700);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(preprocessingWindow->sizePolicy().hasHeightForWidth());
        preprocessingWindow->setSizePolicy(sizePolicy);
        preprocessingWindow->setMaximumSize(QSize(950, 700));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush);
        QBrush brush1(QColor(191, 191, 191, 255));
        brush1.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush1);
        QBrush brush2(QColor(255, 255, 255, 255));
        brush2.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush2);
        QBrush brush3(QColor(223, 223, 223, 255));
        brush3.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush3);
        QBrush brush4(QColor(96, 96, 96, 255));
        brush4.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush4);
        QBrush brush5(QColor(127, 127, 127, 255));
        brush5.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush5);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush2);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush2);
        QBrush brush6(QColor(242, 242, 242, 255));
        brush6.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush6);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush3);
        QBrush brush7(QColor(255, 255, 220, 255));
        brush7.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush);
        QBrush brush8(QColor(0, 0, 0, 127));
        brush8.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush8);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush2);
#endif
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush2);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush3);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush4);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush5);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush2);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush2);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush6);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush3);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush8);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush2);
#endif
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush4);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush2);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush3);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush4);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush5);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush4);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush2);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush4);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush6);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush6);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush);
        QBrush brush9(QColor(96, 96, 96, 127));
        brush9.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush9);
#endif
        QBrush brush10(QColor(249, 249, 249, 255));
        brush10.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush10);
#endif
        preprocessingWindow->setPalette(palette);
        preprocessingWindow->setAcceptDrops(false);
        centralWidget = new QWidget(preprocessingWindow);
        centralWidget->setObjectName("centralWidget");
        centralWidget->setGeometry(QRect(0, 0, 950, 700));
        sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy);
        centralWidget->setMaximumSize(QSize(950, 700));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setHorizontalSpacing(10);
        gridLayout->setVerticalSpacing(0);
        gridLayout->setContentsMargins(10, 10, 10, 10);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(5);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setSizeConstraint(QLayout::SizeConstraint::SetNoConstraint);
        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setSpacing(0);
        verticalLayout_7->setObjectName("verticalLayout_7");
        groupBox_5 = new QGroupBox(centralWidget);
        groupBox_5->setObjectName("groupBox_5");
        QFont font;
        font.setBold(true);
        groupBox_5->setFont(font);
        BPFileSelectListView = new QListView(groupBox_5);
        BPFileSelectListView->setObjectName("BPFileSelectListView");
        BPFileSelectListView->setGeometry(QRect(10, 30, 295, 25));
        sizePolicy.setHeightForWidth(BPFileSelectListView->sizePolicy().hasHeightForWidth());
        BPFileSelectListView->setSizePolicy(sizePolicy);
        BPFileSelectListView->setMinimumSize(QSize(295, 10));
        BPFileSelectListView->setMaximumSize(QSize(295, 25));
        QFont font1;
        font1.setBold(false);
        BPFileSelectListView->setFont(font1);
        BPFileSelectListView->setTabKeyNavigation(true);
        BPFileSelectListView->setDragEnabled(false);
        BPFileSelectListView->setAlternatingRowColors(true);
        BPFileSelectListView->setMovement(QListView::Movement::Static);
        TrialFileSelectListView = new QListView(groupBox_5);
        TrialFileSelectListView->setObjectName("TrialFileSelectListView");
        TrialFileSelectListView->setGeometry(QRect(10, 63, 295, 250));
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(TrialFileSelectListView->sizePolicy().hasHeightForWidth());
        TrialFileSelectListView->setSizePolicy(sizePolicy1);
        TrialFileSelectListView->setMinimumSize(QSize(295, 250));
        TrialFileSelectListView->setMaximumSize(QSize(295, 250));
        TrialFileSelectListView->setFont(font1);
        TrialFileSelectListView->setMidLineWidth(1);
        TrialFileSelectListView->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);
        TrialFileSelectListView->setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked|QAbstractItemView::EditTrigger::EditKeyPressed|QAbstractItemView::EditTrigger::SelectedClicked);
        TrialFileSelectListView->setTabKeyNavigation(true);
        TrialFileSelectListView->setAlternatingRowColors(false);
        TrialFileSelectListView->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
        TrialFileSelectListView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        TrialFileSelectListView->setSelectionRectVisible(true);
        verticalLayoutWidget_3 = new QWidget(groupBox_5);
        verticalLayoutWidget_3->setObjectName("verticalLayoutWidget_3");
        verticalLayoutWidget_3->setGeometry(QRect(310, 19, 131, 111));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget_3);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        pushButton2 = new QPushButton(verticalLayoutWidget_3);
        pushButton2->setObjectName("pushButton2");
        pushButton2->setFont(font1);

        verticalLayout->addWidget(pushButton2);

        pushButton3 = new QPushButton(verticalLayoutWidget_3);
        pushButton3->setObjectName("pushButton3");
        pushButton3->setFont(font1);

        verticalLayout->addWidget(pushButton3);

        removeButton = new QPushButton(verticalLayoutWidget_3);
        removeButton->setObjectName("removeButton");
        removeButton->setFont(font1);

        verticalLayout->addWidget(removeButton);


        verticalLayout_7->addWidget(groupBox_5);

        groupBox_4 = new QGroupBox(centralWidget);
        groupBox_4->setObjectName("groupBox_4");
        groupBox_4->setFont(font);
        verticalLayoutWidget_2 = new QWidget(groupBox_4);
        verticalLayoutWidget_2->setObjectName("verticalLayoutWidget_2");
        verticalLayoutWidget_2->setGeometry(QRect(40, 50, 381, 201));
        hl3D = new QVBoxLayout(verticalLayoutWidget_2);
        hl3D->setSpacing(0);
        hl3D->setObjectName("hl3D");
        hl3D->setSizeConstraint(QLayout::SizeConstraint::SetNoConstraint);
        hl3D->setContentsMargins(0, 0, 0, 0);
        plotLegendWidget = new QWidget(groupBox_4);
        plotLegendWidget->setObjectName("plotLegendWidget");
        plotLegendWidget->setGeometry(QRect(10, 260, 440, 50));
        sizePolicy.setHeightForWidth(plotLegendWidget->sizePolicy().hasHeightForWidth());
        plotLegendWidget->setSizePolicy(sizePolicy);
        plotLegendWidget->setMaximumSize(QSize(440, 50));
        plotLegendWidget->setFont(font1);
        plotTitleWidget = new QLabel(groupBox_4);
        plotTitleWidget->setObjectName("plotTitleWidget");
        plotTitleWidget->setGeometry(QRect(10, 20, 440, 25));
        sizePolicy.setHeightForWidth(plotTitleWidget->sizePolicy().hasHeightForWidth());
        plotTitleWidget->setSizePolicy(sizePolicy);
        plotTitleWidget->setMinimumSize(QSize(440, 25));
        plotTitleWidget->setFont(font1);

        verticalLayout_7->addWidget(groupBox_4);


        horizontalLayout_3->addLayout(verticalLayout_7);

        line = new QFrame(centralWidget);
        line->setObjectName("line");
        line->setFrameShape(QFrame::Shape::VLine);
        line->setFrameShadow(QFrame::Shadow::Sunken);

        horizontalLayout_3->addWidget(line);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName("verticalLayout_4");
        groupBox_3 = new QGroupBox(centralWidget);
        groupBox_3->setObjectName("groupBox_3");
        groupBox_3->setFont(font);
        verticalLayoutWidget = new QWidget(groupBox_3);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(10, 20, 431, 251));
        verticalLayout_12 = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout_12->setSpacing(5);
        verticalLayout_12->setObjectName("verticalLayout_12");
        verticalLayout_12->setSizeConstraint(QLayout::SizeConstraint::SetNoConstraint);
        verticalLayout_12->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        hL01 = new QHBoxLayout();
        hL01->setSpacing(5);
        hL01->setObjectName("hL01");
        hL01->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        checkBox1 = new QCheckBox(verticalLayoutWidget);
        checkBox1->setObjectName("checkBox1");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(checkBox1->sizePolicy().hasHeightForWidth());
        checkBox1->setSizePolicy(sizePolicy2);
        checkBox1->setMinimumSize(QSize(0, 22));
        checkBox1->setFont(font1);
        checkBox1->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        hL01->addWidget(checkBox1);

        horizontalSpacer1 = new QSpacerItem(5, 0, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        hL01->addItem(horizontalSpacer1);

        comboBox1 = new QComboBox(verticalLayoutWidget);
        comboBox1->setObjectName("comboBox1");
        comboBox1->setEnabled(false);
        sizePolicy2.setHeightForWidth(comboBox1->sizePolicy().hasHeightForWidth());
        comboBox1->setSizePolicy(sizePolicy2);
        comboBox1->setMinimumSize(QSize(0, 22));
        comboBox1->setFont(font1);
        comboBox1->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
        comboBox1->setAutoFillBackground(false);
        comboBox1->setEditable(true);
        comboBox1->setFrame(true);

        hL01->addWidget(comboBox1);


        horizontalLayout_2->addLayout(hL01);

        hL09 = new QHBoxLayout();
        hL09->setSpacing(5);
        hL09->setObjectName("hL09");
        checkBox9 = new QCheckBox(verticalLayoutWidget);
        checkBox9->setObjectName("checkBox9");
        sizePolicy2.setHeightForWidth(checkBox9->sizePolicy().hasHeightForWidth());
        checkBox9->setSizePolicy(sizePolicy2);
        checkBox9->setMinimumSize(QSize(0, 22));
        checkBox9->setFont(font1);
        checkBox9->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        hL09->addWidget(checkBox9);

        horizontalSpacer9 = new QSpacerItem(5, 0, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        hL09->addItem(horizontalSpacer9);

        comboBox9 = new QComboBox(verticalLayoutWidget);
        comboBox9->setObjectName("comboBox9");
        comboBox9->setEnabled(false);
        sizePolicy2.setHeightForWidth(comboBox9->sizePolicy().hasHeightForWidth());
        comboBox9->setSizePolicy(sizePolicy2);
        comboBox9->setMinimumSize(QSize(0, 22));
        comboBox9->setFont(font1);
        comboBox9->setEditable(true);

        hL09->addWidget(comboBox9);


        horizontalLayout_2->addLayout(hL09);


        verticalLayout_12->addLayout(horizontalLayout_2);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        hL02 = new QHBoxLayout();
        hL02->setSpacing(5);
        hL02->setObjectName("hL02");
        checkBox2 = new QCheckBox(verticalLayoutWidget);
        checkBox2->setObjectName("checkBox2");
        sizePolicy2.setHeightForWidth(checkBox2->sizePolicy().hasHeightForWidth());
        checkBox2->setSizePolicy(sizePolicy2);
        checkBox2->setMinimumSize(QSize(0, 22));
        checkBox2->setFont(font1);
        checkBox2->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        hL02->addWidget(checkBox2);

        horizontalSpacer2 = new QSpacerItem(5, 0, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        hL02->addItem(horizontalSpacer2);

        comboBox2 = new QComboBox(verticalLayoutWidget);
        comboBox2->setObjectName("comboBox2");
        comboBox2->setEnabled(false);
        sizePolicy2.setHeightForWidth(comboBox2->sizePolicy().hasHeightForWidth());
        comboBox2->setSizePolicy(sizePolicy2);
        comboBox2->setMinimumSize(QSize(0, 22));
        comboBox2->setFont(font1);
        comboBox2->setEditable(true);

        hL02->addWidget(comboBox2);


        horizontalLayout_7->addLayout(hL02);

        hL10 = new QHBoxLayout();
        hL10->setSpacing(5);
        hL10->setObjectName("hL10");
        checkBox10 = new QCheckBox(verticalLayoutWidget);
        checkBox10->setObjectName("checkBox10");
        sizePolicy2.setHeightForWidth(checkBox10->sizePolicy().hasHeightForWidth());
        checkBox10->setSizePolicy(sizePolicy2);
        checkBox10->setMinimumSize(QSize(0, 22));
        checkBox10->setFont(font1);
        checkBox10->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        hL10->addWidget(checkBox10);

        horizontalSpacer10 = new QSpacerItem(5, 0, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        hL10->addItem(horizontalSpacer10);

        comboBox10 = new QComboBox(verticalLayoutWidget);
        comboBox10->setObjectName("comboBox10");
        comboBox10->setEnabled(false);
        sizePolicy2.setHeightForWidth(comboBox10->sizePolicy().hasHeightForWidth());
        comboBox10->setSizePolicy(sizePolicy2);
        comboBox10->setMinimumSize(QSize(0, 22));
        comboBox10->setFont(font1);
        comboBox10->setEditable(true);

        hL10->addWidget(comboBox10);


        horizontalLayout_7->addLayout(hL10);


        verticalLayout_12->addLayout(horizontalLayout_7);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        hL03 = new QHBoxLayout();
        hL03->setSpacing(5);
        hL03->setObjectName("hL03");
        checkBox3 = new QCheckBox(verticalLayoutWidget);
        checkBox3->setObjectName("checkBox3");
        sizePolicy2.setHeightForWidth(checkBox3->sizePolicy().hasHeightForWidth());
        checkBox3->setSizePolicy(sizePolicy2);
        checkBox3->setMinimumSize(QSize(0, 22));
        checkBox3->setFont(font1);
        checkBox3->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        hL03->addWidget(checkBox3);

        horizontalSpacer3 = new QSpacerItem(5, 0, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        hL03->addItem(horizontalSpacer3);

        comboBox3 = new QComboBox(verticalLayoutWidget);
        comboBox3->setObjectName("comboBox3");
        comboBox3->setEnabled(false);
        sizePolicy2.setHeightForWidth(comboBox3->sizePolicy().hasHeightForWidth());
        comboBox3->setSizePolicy(sizePolicy2);
        comboBox3->setMinimumSize(QSize(0, 22));
        comboBox3->setFont(font1);
        comboBox3->setEditable(true);

        hL03->addWidget(comboBox3);


        horizontalLayout_9->addLayout(hL03);

        hL11 = new QHBoxLayout();
        hL11->setSpacing(5);
        hL11->setObjectName("hL11");
        checkBox11 = new QCheckBox(verticalLayoutWidget);
        checkBox11->setObjectName("checkBox11");
        sizePolicy2.setHeightForWidth(checkBox11->sizePolicy().hasHeightForWidth());
        checkBox11->setSizePolicy(sizePolicy2);
        checkBox11->setMinimumSize(QSize(0, 22));
        checkBox11->setFont(font1);
        checkBox11->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        hL11->addWidget(checkBox11);

        horizontalSpacer11 = new QSpacerItem(5, 0, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        hL11->addItem(horizontalSpacer11);

        comboBox11 = new QComboBox(verticalLayoutWidget);
        comboBox11->setObjectName("comboBox11");
        comboBox11->setEnabled(false);
        sizePolicy2.setHeightForWidth(comboBox11->sizePolicy().hasHeightForWidth());
        comboBox11->setSizePolicy(sizePolicy2);
        comboBox11->setMinimumSize(QSize(0, 22));
        comboBox11->setFont(font1);
        comboBox11->setEditable(true);

        hL11->addWidget(comboBox11);


        horizontalLayout_9->addLayout(hL11);


        verticalLayout_12->addLayout(horizontalLayout_9);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        hL04 = new QHBoxLayout();
        hL04->setSpacing(5);
        hL04->setObjectName("hL04");
        checkBox4 = new QCheckBox(verticalLayoutWidget);
        checkBox4->setObjectName("checkBox4");
        sizePolicy2.setHeightForWidth(checkBox4->sizePolicy().hasHeightForWidth());
        checkBox4->setSizePolicy(sizePolicy2);
        checkBox4->setMinimumSize(QSize(0, 22));
        checkBox4->setFont(font1);
        checkBox4->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        hL04->addWidget(checkBox4);

        horizontalSpacer4 = new QSpacerItem(5, 0, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        hL04->addItem(horizontalSpacer4);

        comboBox4 = new QComboBox(verticalLayoutWidget);
        comboBox4->setObjectName("comboBox4");
        comboBox4->setEnabled(false);
        sizePolicy2.setHeightForWidth(comboBox4->sizePolicy().hasHeightForWidth());
        comboBox4->setSizePolicy(sizePolicy2);
        comboBox4->setMinimumSize(QSize(0, 22));
        comboBox4->setFont(font1);
        comboBox4->setEditable(true);

        hL04->addWidget(comboBox4);


        horizontalLayout_10->addLayout(hL04);

        hL12 = new QHBoxLayout();
        hL12->setSpacing(5);
        hL12->setObjectName("hL12");
        checkBox12 = new QCheckBox(verticalLayoutWidget);
        checkBox12->setObjectName("checkBox12");
        sizePolicy2.setHeightForWidth(checkBox12->sizePolicy().hasHeightForWidth());
        checkBox12->setSizePolicy(sizePolicy2);
        checkBox12->setMinimumSize(QSize(0, 22));
        checkBox12->setFont(font1);
        checkBox12->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        hL12->addWidget(checkBox12);

        horizontalSpacer12 = new QSpacerItem(5, 0, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        hL12->addItem(horizontalSpacer12);

        comboBox12 = new QComboBox(verticalLayoutWidget);
        comboBox12->setObjectName("comboBox12");
        comboBox12->setEnabled(false);
        sizePolicy2.setHeightForWidth(comboBox12->sizePolicy().hasHeightForWidth());
        comboBox12->setSizePolicy(sizePolicy2);
        comboBox12->setMinimumSize(QSize(0, 22));
        comboBox12->setFont(font1);
        comboBox12->setEditable(true);

        hL12->addWidget(comboBox12);


        horizontalLayout_10->addLayout(hL12);


        verticalLayout_12->addLayout(horizontalLayout_10);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName("horizontalLayout_11");
        hL05 = new QHBoxLayout();
        hL05->setSpacing(5);
        hL05->setObjectName("hL05");
        checkBox5 = new QCheckBox(verticalLayoutWidget);
        checkBox5->setObjectName("checkBox5");
        sizePolicy2.setHeightForWidth(checkBox5->sizePolicy().hasHeightForWidth());
        checkBox5->setSizePolicy(sizePolicy2);
        checkBox5->setMinimumSize(QSize(0, 22));
        checkBox5->setFont(font1);
        checkBox5->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        hL05->addWidget(checkBox5);

        horizontalSpacer5 = new QSpacerItem(5, 0, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        hL05->addItem(horizontalSpacer5);

        comboBox5 = new QComboBox(verticalLayoutWidget);
        comboBox5->setObjectName("comboBox5");
        comboBox5->setEnabled(false);
        sizePolicy2.setHeightForWidth(comboBox5->sizePolicy().hasHeightForWidth());
        comboBox5->setSizePolicy(sizePolicy2);
        comboBox5->setMinimumSize(QSize(0, 22));
        comboBox5->setFont(font1);
        comboBox5->setEditable(true);

        hL05->addWidget(comboBox5);


        horizontalLayout_11->addLayout(hL05);

        hL13 = new QHBoxLayout();
        hL13->setSpacing(5);
        hL13->setObjectName("hL13");
        checkBox13 = new QCheckBox(verticalLayoutWidget);
        checkBox13->setObjectName("checkBox13");
        sizePolicy2.setHeightForWidth(checkBox13->sizePolicy().hasHeightForWidth());
        checkBox13->setSizePolicy(sizePolicy2);
        checkBox13->setMinimumSize(QSize(0, 22));
        checkBox13->setFont(font1);
        checkBox13->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        hL13->addWidget(checkBox13);

        horizontalSpacer13 = new QSpacerItem(5, 0, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        hL13->addItem(horizontalSpacer13);

        comboBox13 = new QComboBox(verticalLayoutWidget);
        comboBox13->setObjectName("comboBox13");
        comboBox13->setEnabled(false);
        sizePolicy2.setHeightForWidth(comboBox13->sizePolicy().hasHeightForWidth());
        comboBox13->setSizePolicy(sizePolicy2);
        comboBox13->setMinimumSize(QSize(0, 22));
        comboBox13->setFont(font1);
        comboBox13->setEditable(true);

        hL13->addWidget(comboBox13);


        horizontalLayout_11->addLayout(hL13);


        verticalLayout_12->addLayout(horizontalLayout_11);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName("horizontalLayout_12");
        hL06 = new QHBoxLayout();
        hL06->setSpacing(5);
        hL06->setObjectName("hL06");
        checkBox6 = new QCheckBox(verticalLayoutWidget);
        checkBox6->setObjectName("checkBox6");
        sizePolicy2.setHeightForWidth(checkBox6->sizePolicy().hasHeightForWidth());
        checkBox6->setSizePolicy(sizePolicy2);
        checkBox6->setMinimumSize(QSize(0, 22));
        checkBox6->setFont(font1);
        checkBox6->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        hL06->addWidget(checkBox6);

        horizontalSpacer6 = new QSpacerItem(5, 0, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        hL06->addItem(horizontalSpacer6);

        comboBox6 = new QComboBox(verticalLayoutWidget);
        comboBox6->setObjectName("comboBox6");
        comboBox6->setEnabled(false);
        sizePolicy2.setHeightForWidth(comboBox6->sizePolicy().hasHeightForWidth());
        comboBox6->setSizePolicy(sizePolicy2);
        comboBox6->setMinimumSize(QSize(0, 22));
        comboBox6->setFont(font1);
        comboBox6->setEditable(true);

        hL06->addWidget(comboBox6);


        horizontalLayout_12->addLayout(hL06);

        hL14 = new QHBoxLayout();
        hL14->setSpacing(5);
        hL14->setObjectName("hL14");
        checkBox14 = new QCheckBox(verticalLayoutWidget);
        checkBox14->setObjectName("checkBox14");
        sizePolicy2.setHeightForWidth(checkBox14->sizePolicy().hasHeightForWidth());
        checkBox14->setSizePolicy(sizePolicy2);
        checkBox14->setMinimumSize(QSize(0, 22));
        checkBox14->setFont(font1);
        checkBox14->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        hL14->addWidget(checkBox14);

        horizontalSpacer14 = new QSpacerItem(5, 0, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        hL14->addItem(horizontalSpacer14);

        comboBox14 = new QComboBox(verticalLayoutWidget);
        comboBox14->setObjectName("comboBox14");
        comboBox14->setEnabled(false);
        sizePolicy2.setHeightForWidth(comboBox14->sizePolicy().hasHeightForWidth());
        comboBox14->setSizePolicy(sizePolicy2);
        comboBox14->setMinimumSize(QSize(0, 22));
        comboBox14->setFont(font1);
        comboBox14->setEditable(true);

        hL14->addWidget(comboBox14);


        horizontalLayout_12->addLayout(hL14);


        verticalLayout_12->addLayout(horizontalLayout_12);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName("horizontalLayout_13");
        hL07 = new QHBoxLayout();
        hL07->setSpacing(5);
        hL07->setObjectName("hL07");
        checkBox7 = new QCheckBox(verticalLayoutWidget);
        checkBox7->setObjectName("checkBox7");
        sizePolicy2.setHeightForWidth(checkBox7->sizePolicy().hasHeightForWidth());
        checkBox7->setSizePolicy(sizePolicy2);
        checkBox7->setMinimumSize(QSize(0, 22));
        checkBox7->setFont(font1);
        checkBox7->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        hL07->addWidget(checkBox7);

        horizontalSpacer7 = new QSpacerItem(5, 0, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        hL07->addItem(horizontalSpacer7);

        comboBox7 = new QComboBox(verticalLayoutWidget);
        comboBox7->setObjectName("comboBox7");
        comboBox7->setEnabled(false);
        sizePolicy2.setHeightForWidth(comboBox7->sizePolicy().hasHeightForWidth());
        comboBox7->setSizePolicy(sizePolicy2);
        comboBox7->setMinimumSize(QSize(0, 22));
        comboBox7->setFont(font1);
        comboBox7->setEditable(true);

        hL07->addWidget(comboBox7);


        horizontalLayout_13->addLayout(hL07);

        hL15 = new QHBoxLayout();
        hL15->setSpacing(5);
        hL15->setObjectName("hL15");
        checkBox15 = new QCheckBox(verticalLayoutWidget);
        checkBox15->setObjectName("checkBox15");
        sizePolicy2.setHeightForWidth(checkBox15->sizePolicy().hasHeightForWidth());
        checkBox15->setSizePolicy(sizePolicy2);
        checkBox15->setMinimumSize(QSize(0, 22));
        checkBox15->setFont(font1);
        checkBox15->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        hL15->addWidget(checkBox15);

        horizontalSpacer15 = new QSpacerItem(5, 0, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        hL15->addItem(horizontalSpacer15);

        comboBox15 = new QComboBox(verticalLayoutWidget);
        comboBox15->setObjectName("comboBox15");
        comboBox15->setEnabled(false);
        sizePolicy2.setHeightForWidth(comboBox15->sizePolicy().hasHeightForWidth());
        comboBox15->setSizePolicy(sizePolicy2);
        comboBox15->setMinimumSize(QSize(0, 22));
        comboBox15->setFont(font1);
        comboBox15->setEditable(true);

        hL15->addWidget(comboBox15);


        horizontalLayout_13->addLayout(hL15);


        verticalLayout_12->addLayout(horizontalLayout_13);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName("horizontalLayout_14");
        hL08 = new QHBoxLayout();
        hL08->setSpacing(5);
        hL08->setObjectName("hL08");
        checkBox8 = new QCheckBox(verticalLayoutWidget);
        checkBox8->setObjectName("checkBox8");
        sizePolicy2.setHeightForWidth(checkBox8->sizePolicy().hasHeightForWidth());
        checkBox8->setSizePolicy(sizePolicy2);
        checkBox8->setMinimumSize(QSize(0, 22));
        checkBox8->setFont(font1);
        checkBox8->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        hL08->addWidget(checkBox8);

        horizontalSpacer8 = new QSpacerItem(5, 0, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        hL08->addItem(horizontalSpacer8);

        comboBox8 = new QComboBox(verticalLayoutWidget);
        comboBox8->setObjectName("comboBox8");
        comboBox8->setEnabled(false);
        sizePolicy2.setHeightForWidth(comboBox8->sizePolicy().hasHeightForWidth());
        comboBox8->setSizePolicy(sizePolicy2);
        comboBox8->setMinimumSize(QSize(0, 22));
        comboBox8->setFont(font1);
        comboBox8->setEditable(true);

        hL08->addWidget(comboBox8);


        horizontalLayout_14->addLayout(hL08);

        hL16 = new QHBoxLayout();
        hL16->setSpacing(5);
        hL16->setObjectName("hL16");
        checkBox16 = new QCheckBox(verticalLayoutWidget);
        checkBox16->setObjectName("checkBox16");
        sizePolicy2.setHeightForWidth(checkBox16->sizePolicy().hasHeightForWidth());
        checkBox16->setSizePolicy(sizePolicy2);
        checkBox16->setMinimumSize(QSize(0, 22));
        checkBox16->setFont(font1);
        checkBox16->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        hL16->addWidget(checkBox16);

        horizontalSpacer16 = new QSpacerItem(5, 0, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        hL16->addItem(horizontalSpacer16);

        comboBox16 = new QComboBox(verticalLayoutWidget);
        comboBox16->setObjectName("comboBox16");
        comboBox16->setEnabled(false);
        sizePolicy2.setHeightForWidth(comboBox16->sizePolicy().hasHeightForWidth());
        comboBox16->setSizePolicy(sizePolicy2);
        comboBox16->setMinimumSize(QSize(0, 22));
        comboBox16->setFont(font1);
        comboBox16->setEditable(true);

        hL16->addWidget(comboBox16);


        horizontalLayout_14->addLayout(hL16);


        verticalLayout_12->addLayout(horizontalLayout_14);

        groupBox = new QGroupBox(groupBox_3);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(10, 280, 431, 81));
        groupBox->setFont(font);
        label_13 = new QLabel(groupBox);
        label_13->setObjectName("label_13");
        label_13->setGeometry(QRect(20, 30, 70, 15));
        sizePolicy2.setHeightForWidth(label_13->sizePolicy().hasHeightForWidth());
        label_13->setSizePolicy(sizePolicy2);
        QPalette palette1;
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush2);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush2);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush2);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush5);
        QBrush brush11(QColor(170, 170, 170, 255));
        brush11.setStyle(Qt::BrushStyle::SolidPattern);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush11);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush2);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush2);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush2);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush2);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush8);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush2);
#endif
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush2);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush2);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush2);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush5);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush11);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush2);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush2);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush2);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush2);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush8);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush2);
#endif
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush5);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush2);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush2);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush2);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush5);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush11);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush5);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush2);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush5);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush2);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush2);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush2);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush);
        QBrush brush12(QColor(127, 127, 127, 127));
        brush12.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush12);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush2);
#endif
        label_13->setPalette(palette1);
        label_13->setFont(font1);
        label_13->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);
        bpLeftCombo = new QComboBox(groupBox);
        bpLeftCombo->setObjectName("bpLeftCombo");
        bpLeftCombo->setEnabled(false);
        bpLeftCombo->setGeometry(QRect(10, 50, 101, 22));
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(bpLeftCombo->sizePolicy().hasHeightForWidth());
        bpLeftCombo->setSizePolicy(sizePolicy3);
        bpLeftCombo->setMinimumSize(QSize(0, 0));
        bpLeftCombo->setFont(font1);
        bpLeftCombo->setEditable(true);
        label_14 = new QLabel(groupBox);
        label_14->setObjectName("label_14");
        label_14->setGeometry(QRect(130, 30, 70, 15));
        sizePolicy2.setHeightForWidth(label_14->sizePolicy().hasHeightForWidth());
        label_14->setSizePolicy(sizePolicy2);
        QPalette palette2;
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush2);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush2);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush2);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush5);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush11);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush2);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush2);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush2);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush2);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush8);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush2);
#endif
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush2);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush2);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush2);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush5);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush11);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush2);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush2);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush2);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush2);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush8);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush2);
#endif
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush5);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush2);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush2);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush2);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush5);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush11);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush5);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush2);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush5);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush2);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush2);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush2);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush12);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush2);
#endif
        label_14->setPalette(palette2);
        label_14->setFont(font1);
        label_14->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);
        bpRightCombo = new QComboBox(groupBox);
        bpRightCombo->setObjectName("bpRightCombo");
        bpRightCombo->setEnabled(false);
        bpRightCombo->setGeometry(QRect(120, 50, 101, 22));
        sizePolicy3.setHeightForWidth(bpRightCombo->sizePolicy().hasHeightForWidth());
        bpRightCombo->setSizePolicy(sizePolicy3);
        bpRightCombo->setMinimumSize(QSize(0, 0));
        bpRightCombo->setFont(font1);
        bpRightCombo->setEditable(true);
        label_15 = new QLabel(groupBox);
        label_15->setObjectName("label_15");
        label_15->setGeometry(QRect(240, 30, 70, 15));
        sizePolicy2.setHeightForWidth(label_15->sizePolicy().hasHeightForWidth());
        label_15->setSizePolicy(sizePolicy2);
        QPalette palette3;
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush2);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush2);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush2);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush5);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush11);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush2);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush2);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush2);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush2);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush8);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush2);
#endif
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush2);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush2);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush2);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush5);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush11);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush2);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush2);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush2);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush2);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush8);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush2);
#endif
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush5);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush2);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush2);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush2);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush5);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush11);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush5);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush2);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush5);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush2);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush2);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush2);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush12);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush2);
#endif
        label_15->setPalette(palette3);
        label_15->setFont(font1);
        label_15->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);
        bpFrontCombo = new QComboBox(groupBox);
        bpFrontCombo->setObjectName("bpFrontCombo");
        bpFrontCombo->setEnabled(false);
        bpFrontCombo->setGeometry(QRect(230, 50, 101, 22));
        sizePolicy3.setHeightForWidth(bpFrontCombo->sizePolicy().hasHeightForWidth());
        bpFrontCombo->setSizePolicy(sizePolicy3);
        bpFrontCombo->setMinimumSize(QSize(0, 0));
        bpFrontCombo->setFont(font1);
        bpFrontCombo->setEditable(true);

        verticalLayout_4->addWidget(groupBox_3);

        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setMaximumSize(QSize(16777215, 200));
        groupBox_2->setFont(font);
        layoutWidget = new QWidget(groupBox_2);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 170, 431, 32));
        horizontalLayout_5 = new QHBoxLayout(layoutWidget);
        horizontalLayout_5->setSpacing(2);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setSizeConstraint(QLayout::SizeConstraint::SetNoConstraint);
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);

        startButton = new QPushButton(layoutWidget);
        startButton->setObjectName("startButton");
        QSizePolicy sizePolicy4(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(startButton->sizePolicy().hasHeightForWidth());
        startButton->setSizePolicy(sizePolicy4);
        startButton->setMinimumSize(QSize(0, 0));
        startButton->setMaximumSize(QSize(16777215, 16777215));
        startButton->setFont(font1);
        startButton->setIconSize(QSize(16, 16));

        horizontalLayout_5->addWidget(startButton);

        proceedButton = new QPushButton(layoutWidget);
        proceedButton->setObjectName("proceedButton");
        proceedButton->setEnabled(false);
        sizePolicy4.setHeightForWidth(proceedButton->sizePolicy().hasHeightForWidth());
        proceedButton->setSizePolicy(sizePolicy4);
        proceedButton->setMinimumSize(QSize(0, 0));
        proceedButton->setMaximumSize(QSize(16777215, 16777215));
        proceedButton->setFont(font1);

        horizontalLayout_5->addWidget(proceedButton);

        backButton = new QPushButton(layoutWidget);
        backButton->setObjectName("backButton");
        backButton->setFont(font1);

        horizontalLayout_5->addWidget(backButton);

        backButton_2 = new QPushButton(layoutWidget);
        backButton_2->setObjectName("backButton_2");
        backButton_2->setFont(font1);

        horizontalLayout_5->addWidget(backButton_2);

        cancelButton = new QPushButton(layoutWidget);
        cancelButton->setObjectName("cancelButton");
        sizePolicy4.setHeightForWidth(cancelButton->sizePolicy().hasHeightForWidth());
        cancelButton->setSizePolicy(sizePolicy4);
        cancelButton->setMinimumSize(QSize(0, 0));
        cancelButton->setMaximumSize(QSize(16777215, 16777215));
        cancelButton->setFont(font1);
        cancelButton->setFlat(false);

        horizontalLayout_5->addWidget(cancelButton);

        messagesListView = new QListView(groupBox_2);
        messagesListView->setObjectName("messagesListView");
        messagesListView->setGeometry(QRect(10, 20, 441, 150));
        QSizePolicy sizePolicy5(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(messagesListView->sizePolicy().hasHeightForWidth());
        messagesListView->setSizePolicy(sizePolicy5);
        messagesListView->setMinimumSize(QSize(350, 150));
        messagesListView->setMaximumSize(QSize(16777215, 150));
        messagesListView->setFont(font1);

        verticalLayout_4->addWidget(groupBox_2);


        horizontalLayout_3->addLayout(verticalLayout_4);


        gridLayout->addLayout(horizontalLayout_3, 4, 1, 1, 1);


        retranslateUi(preprocessingWindow);

        cancelButton->setDefault(false);


        QMetaObject::connectSlotsByName(preprocessingWindow);
    } // setupUi

    void retranslateUi(QWidget *preprocessingWindow)
    {
        preprocessingWindow->setWindowTitle(QCoreApplication::translate("preprocessingWindow", "Pre-processing Window", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("preprocessingWindow", "Files", nullptr));
        pushButton2->setText(QCoreApplication::translate("preprocessingWindow", "Select BP File", nullptr));
        pushButton3->setText(QCoreApplication::translate("preprocessingWindow", "Select Trial Files", nullptr));
        removeButton->setText(QCoreApplication::translate("preprocessingWindow", "Remove", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("preprocessingWindow", "Sensor View", nullptr));
        plotTitleWidget->setText(QString());
        groupBox_3->setTitle(QCoreApplication::translate("preprocessingWindow", "Sensor Layout", nullptr));
        checkBox1->setText(QCoreApplication::translate("preprocessingWindow", "Channel 1", nullptr));
        checkBox9->setText(QCoreApplication::translate("preprocessingWindow", "Channel 9 ", nullptr));
        checkBox2->setText(QCoreApplication::translate("preprocessingWindow", "Channel 2", nullptr));
        checkBox10->setText(QCoreApplication::translate("preprocessingWindow", "Channel 10", nullptr));
        checkBox3->setText(QCoreApplication::translate("preprocessingWindow", "Channel 3", nullptr));
        checkBox11->setText(QCoreApplication::translate("preprocessingWindow", "Channel 11", nullptr));
        checkBox4->setText(QCoreApplication::translate("preprocessingWindow", "Channel 4", nullptr));
        checkBox12->setText(QCoreApplication::translate("preprocessingWindow", "Channel 12", nullptr));
        checkBox5->setText(QCoreApplication::translate("preprocessingWindow", "Channel 5", nullptr));
        checkBox13->setText(QCoreApplication::translate("preprocessingWindow", "Channel 13", nullptr));
        checkBox6->setText(QCoreApplication::translate("preprocessingWindow", "Channel 6", nullptr));
        checkBox14->setText(QCoreApplication::translate("preprocessingWindow", "Channel 14", nullptr));
        checkBox7->setText(QCoreApplication::translate("preprocessingWindow", "Channel 7", nullptr));
        checkBox15->setText(QCoreApplication::translate("preprocessingWindow", "Channel 15", nullptr));
        checkBox8->setText(QCoreApplication::translate("preprocessingWindow", "Channel 8", nullptr));
        checkBox16->setText(QCoreApplication::translate("preprocessingWindow", "Channel 16", nullptr));
        groupBox->setTitle(QCoreApplication::translate("preprocessingWindow", "Bite Plane Channels:", nullptr));
        label_13->setText(QCoreApplication::translate("preprocessingWindow", "BP-L", nullptr));
        label_14->setText(QCoreApplication::translate("preprocessingWindow", "BP-R", nullptr));
        label_15->setText(QCoreApplication::translate("preprocessingWindow", "BP-F", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("preprocessingWindow", "Status Messages", nullptr));
        startButton->setText(QCoreApplication::translate("preprocessingWindow", "Start", nullptr));
        proceedButton->setText(QCoreApplication::translate("preprocessingWindow", "Proceed", nullptr));
        backButton->setText(QCoreApplication::translate("preprocessingWindow", "Back", nullptr));
        backButton_2->setText(QCoreApplication::translate("preprocessingWindow", "Help", nullptr));
        cancelButton->setText(QCoreApplication::translate("preprocessingWindow", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class preprocessingWindow: public Ui_preprocessingWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREPROCESSINGWINDOW_H

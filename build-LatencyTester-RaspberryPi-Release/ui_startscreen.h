/********************************************************************************
** Form generated from reading UI file 'startscreen.ui'
**
** Created by: Qt User Interface Compiler version 5.15.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STARTSCREEN_H
#define UI_STARTSCREEN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_StartScreen
{
public:
    QWidget *startWidget;
    QFrame *helpFrame;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *helpLayout;
    QPushButton *usersManualButton;
    QPushButton *generalInfoButton;
    QFrame *generalInfoFrame;
    QPlainTextEdit *generalInfoText;
    QFrame *settingsFrame;
    QWidget *verticalLayoutWidget_3;
    QVBoxLayout *settingsLayout;
    QLabel *languageLabel;
    QComboBox *languagesComboBox;
    QLabel *fontSizeLabel;
    QSlider *fontSizeSlider;
    QCheckBox *daltonicCheckbox;
    QDialogButtonBox *settingsButtonBox;
    QFrame *registryFrame;
    QTreeView *registryTreeView;
    QGroupBox *registryButtonsBox;
    QPushButton *checkRegistryEntryButton;
    QPushButton *deleteRegistryEntryButton;
    QPushButton *renameRegistryEntryButton;
    QFrame *startFrame;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *startLayout;
    QPushButton *startMeasuringButton;
    QPushButton *historicOfMeasuresButton;
    QPushButton *settingsButton;
    QPushButton *helpButton;
    QPushButton *backButton;
    QFrame *registryDisplayerFrame;
    QWidget *formLayoutWidget_2;
    QFormLayout *formLayout_2;
    QLabel *latencyLabel;
    QLineEdit *latencyText;
    QLabel *timeLabel;
    QLineEdit *timeText;
    QWidget *formLayoutWidget_3;
    QFormLayout *formLayout_3;
    QLabel *nameLabel;
    QLineEdit *nameText;
    QDateTimeEdit *dateTimeEdit;
    QLabel *dateLabel;
    QCustomPlot *plotMeasures;
    QFrame *startMeasureFrame;
    QWidget *verticalLayoutWidget_4;
    QVBoxLayout *startMeasureVerticalLayoutLeft;
    QLabel *timeFactorLabel;
    QSlider *TimeFactorSlider;
    QWidget *verticalLayoutWidget_5;
    QVBoxLayout *startMeasureVerticalLayoutRight;
    QLabel *DurationLabel;
    QSlider *DurationSlider;
    QPushButton *calibrateButton;
    QPushButton *startMeasureButton;
    QPushButton *stopMeasureButton;

    void setupUi(QMainWindow *StartScreen)
    {
        if (StartScreen->objectName().isEmpty())
            StartScreen->setObjectName(QString::fromUtf8("StartScreen"));
        StartScreen->resize(800, 480);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(StartScreen->sizePolicy().hasHeightForWidth());
        StartScreen->setSizePolicy(sizePolicy);
        StartScreen->setMinimumSize(QSize(800, 480));
        StartScreen->setMaximumSize(QSize(800, 480));
        StartScreen->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 180, 216);\n"
"alternate-background-color: rgb(82, 183, 136);\n"
""));
        StartScreen->setToolButtonStyle(Qt::ToolButtonTextOnly);
        startWidget = new QWidget(StartScreen);
        startWidget->setObjectName(QString::fromUtf8("startWidget"));
        sizePolicy.setHeightForWidth(startWidget->sizePolicy().hasHeightForWidth());
        startWidget->setSizePolicy(sizePolicy);
        startWidget->setMinimumSize(QSize(800, 480));
        startWidget->setMaximumSize(QSize(800, 480));
        helpFrame = new QFrame(startWidget);
        helpFrame->setObjectName(QString::fromUtf8("helpFrame"));
        helpFrame->setEnabled(false);
        helpFrame->setGeometry(QRect(0, 0, 800, 480));
        sizePolicy.setHeightForWidth(helpFrame->sizePolicy().hasHeightForWidth());
        helpFrame->setSizePolicy(sizePolicy);
        helpFrame->setMinimumSize(QSize(800, 480));
        helpFrame->setMaximumSize(QSize(800, 480));
        helpFrame->setFrameShape(QFrame::StyledPanel);
        helpFrame->setFrameShadow(QFrame::Raised);
        verticalLayoutWidget_2 = new QWidget(helpFrame);
        verticalLayoutWidget_2->setObjectName(QString::fromUtf8("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(210, 10, 391, 461));
        helpLayout = new QVBoxLayout(verticalLayoutWidget_2);
        helpLayout->setSpacing(6);
        helpLayout->setObjectName(QString::fromUtf8("helpLayout"));
        helpLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        helpLayout->setContentsMargins(5, 0, 5, 0);
        usersManualButton = new QPushButton(verticalLayoutWidget_2);
        usersManualButton->setObjectName(QString::fromUtf8("usersManualButton"));
        usersManualButton->setEnabled(false);

        helpLayout->addWidget(usersManualButton);

        generalInfoButton = new QPushButton(verticalLayoutWidget_2);
        generalInfoButton->setObjectName(QString::fromUtf8("generalInfoButton"));

        helpLayout->addWidget(generalInfoButton);

        generalInfoFrame = new QFrame(startWidget);
        generalInfoFrame->setObjectName(QString::fromUtf8("generalInfoFrame"));
        generalInfoFrame->setGeometry(QRect(0, 0, 800, 480));
        sizePolicy.setHeightForWidth(generalInfoFrame->sizePolicy().hasHeightForWidth());
        generalInfoFrame->setSizePolicy(sizePolicy);
        generalInfoFrame->setMinimumSize(QSize(800, 480));
        generalInfoFrame->setMaximumSize(QSize(800, 480));
        generalInfoFrame->setFrameShape(QFrame::StyledPanel);
        generalInfoFrame->setFrameShadow(QFrame::Raised);
        generalInfoText = new QPlainTextEdit(generalInfoFrame);
        generalInfoText->setObjectName(QString::fromUtf8("generalInfoText"));
        generalInfoText->setEnabled(false);
        generalInfoText->setGeometry(QRect(230, 60, 311, 341));
        QFont font;
        font.setFamily(QString::fromUtf8("DejaVu Sans"));
        font.setPointSize(12);
        generalInfoText->setFont(font);
        generalInfoText->setTextInteractionFlags(Qt::NoTextInteraction);
        settingsFrame = new QFrame(startWidget);
        settingsFrame->setObjectName(QString::fromUtf8("settingsFrame"));
        settingsFrame->setEnabled(true);
        settingsFrame->setGeometry(QRect(0, 0, 800, 480));
        sizePolicy.setHeightForWidth(settingsFrame->sizePolicy().hasHeightForWidth());
        settingsFrame->setSizePolicy(sizePolicy);
        settingsFrame->setMinimumSize(QSize(800, 480));
        settingsFrame->setMaximumSize(QSize(800, 480));
        settingsFrame->setFrameShape(QFrame::StyledPanel);
        settingsFrame->setFrameShadow(QFrame::Raised);
        verticalLayoutWidget_3 = new QWidget(settingsFrame);
        verticalLayoutWidget_3->setObjectName(QString::fromUtf8("verticalLayoutWidget_3"));
        verticalLayoutWidget_3->setGeometry(QRect(160, 30, 601, 421));
        settingsLayout = new QVBoxLayout(verticalLayoutWidget_3);
        settingsLayout->setObjectName(QString::fromUtf8("settingsLayout"));
        settingsLayout->setSizeConstraint(QLayout::SetNoConstraint);
        settingsLayout->setContentsMargins(0, 0, 0, 0);
        languageLabel = new QLabel(verticalLayoutWidget_3);
        languageLabel->setObjectName(QString::fromUtf8("languageLabel"));
        sizePolicy.setHeightForWidth(languageLabel->sizePolicy().hasHeightForWidth());
        languageLabel->setSizePolicy(sizePolicy);
        languageLabel->setMinimumSize(QSize(2, 0));

        settingsLayout->addWidget(languageLabel, 0, Qt::AlignHCenter);

        languagesComboBox = new QComboBox(verticalLayoutWidget_3);
        languagesComboBox->addItem(QString());
        languagesComboBox->addItem(QString());
        languagesComboBox->addItem(QString());
        languagesComboBox->setObjectName(QString::fromUtf8("languagesComboBox"));

        settingsLayout->addWidget(languagesComboBox, 0, Qt::AlignHCenter);

        fontSizeLabel = new QLabel(verticalLayoutWidget_3);
        fontSizeLabel->setObjectName(QString::fromUtf8("fontSizeLabel"));
        sizePolicy.setHeightForWidth(fontSizeLabel->sizePolicy().hasHeightForWidth());
        fontSizeLabel->setSizePolicy(sizePolicy);
        fontSizeLabel->setAlignment(Qt::AlignCenter);

        settingsLayout->addWidget(fontSizeLabel, 0, Qt::AlignHCenter);

        fontSizeSlider = new QSlider(verticalLayoutWidget_3);
        fontSizeSlider->setObjectName(QString::fromUtf8("fontSizeSlider"));
        fontSizeSlider->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);"));
        fontSizeSlider->setMinimum(10);
        fontSizeSlider->setMaximum(20);
        fontSizeSlider->setSliderPosition(10);
        fontSizeSlider->setOrientation(Qt::Horizontal);
        fontSizeSlider->setTickPosition(QSlider::TicksBothSides);
        fontSizeSlider->setTickInterval(1);

        settingsLayout->addWidget(fontSizeSlider);

        daltonicCheckbox = new QCheckBox(verticalLayoutWidget_3);
        daltonicCheckbox->setObjectName(QString::fromUtf8("daltonicCheckbox"));

        settingsLayout->addWidget(daltonicCheckbox, 0, Qt::AlignHCenter);

        settingsButtonBox = new QDialogButtonBox(verticalLayoutWidget_3);
        settingsButtonBox->setObjectName(QString::fromUtf8("settingsButtonBox"));
        settingsButtonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Reset);

        settingsLayout->addWidget(settingsButtonBox, 0, Qt::AlignHCenter);

        registryFrame = new QFrame(startWidget);
        registryFrame->setObjectName(QString::fromUtf8("registryFrame"));
        registryFrame->setGeometry(QRect(0, 0, 800, 480));
        sizePolicy.setHeightForWidth(registryFrame->sizePolicy().hasHeightForWidth());
        registryFrame->setSizePolicy(sizePolicy);
        registryFrame->setMinimumSize(QSize(800, 480));
        registryFrame->setMaximumSize(QSize(800, 480));
        registryFrame->setFrameShape(QFrame::StyledPanel);
        registryFrame->setFrameShadow(QFrame::Raised);
        registryTreeView = new QTreeView(registryFrame);
        registryTreeView->setObjectName(QString::fromUtf8("registryTreeView"));
        registryTreeView->setGeometry(QRect(120, 10, 581, 391));
        registryTreeView->setEditTriggers(QAbstractItemView::SelectedClicked);
        registryTreeView->setAutoExpandDelay(-1);
        registryButtonsBox = new QGroupBox(registryFrame);
        registryButtonsBox->setObjectName(QString::fromUtf8("registryButtonsBox"));
        registryButtonsBox->setGeometry(QRect(120, 400, 581, 75));
        registryButtonsBox->setAlignment(Qt::AlignCenter);
        registryButtonsBox->setFlat(false);
        checkRegistryEntryButton = new QPushButton(registryButtonsBox);
        checkRegistryEntryButton->setObjectName(QString::fromUtf8("checkRegistryEntryButton"));
        checkRegistryEntryButton->setGeometry(QRect(20, 30, 83, 25));
        deleteRegistryEntryButton = new QPushButton(registryButtonsBox);
        deleteRegistryEntryButton->setObjectName(QString::fromUtf8("deleteRegistryEntryButton"));
        deleteRegistryEntryButton->setGeometry(QRect(270, 30, 83, 25));
        renameRegistryEntryButton = new QPushButton(registryButtonsBox);
        renameRegistryEntryButton->setObjectName(QString::fromUtf8("renameRegistryEntryButton"));
        renameRegistryEntryButton->setGeometry(QRect(490, 30, 83, 25));
        startFrame = new QFrame(startWidget);
        startFrame->setObjectName(QString::fromUtf8("startFrame"));
        startFrame->setGeometry(QRect(0, 0, 800, 480));
        sizePolicy.setHeightForWidth(startFrame->sizePolicy().hasHeightForWidth());
        startFrame->setSizePolicy(sizePolicy);
        startFrame->setMinimumSize(QSize(800, 480));
        startFrame->setMaximumSize(QSize(800, 480));
        startFrame->setFrameShape(QFrame::StyledPanel);
        startFrame->setFrameShadow(QFrame::Raised);
        verticalLayoutWidget = new QWidget(startFrame);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(190, 10, 391, 461));
        startLayout = new QVBoxLayout(verticalLayoutWidget);
        startLayout->setSpacing(6);
        startLayout->setObjectName(QString::fromUtf8("startLayout"));
        startLayout->setSizeConstraint(QLayout::SetNoConstraint);
        startLayout->setContentsMargins(5, 0, 5, 0);
        startMeasuringButton = new QPushButton(verticalLayoutWidget);
        startMeasuringButton->setObjectName(QString::fromUtf8("startMeasuringButton"));
        startMeasuringButton->setStyleSheet(QString::fromUtf8(""));

        startLayout->addWidget(startMeasuringButton);

        historicOfMeasuresButton = new QPushButton(verticalLayoutWidget);
        historicOfMeasuresButton->setObjectName(QString::fromUtf8("historicOfMeasuresButton"));

        startLayout->addWidget(historicOfMeasuresButton);

        settingsButton = new QPushButton(verticalLayoutWidget);
        settingsButton->setObjectName(QString::fromUtf8("settingsButton"));

        startLayout->addWidget(settingsButton);

        helpButton = new QPushButton(verticalLayoutWidget);
        helpButton->setObjectName(QString::fromUtf8("helpButton"));

        startLayout->addWidget(helpButton);

        backButton = new QPushButton(startWidget);
        backButton->setObjectName(QString::fromUtf8("backButton"));
        backButton->setEnabled(false);
        backButton->setGeometry(QRect(20, 20, 83, 25));
        sizePolicy.setHeightForWidth(backButton->sizePolicy().hasHeightForWidth());
        backButton->setSizePolicy(sizePolicy);
        backButton->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 214, 10);\n"
"alternate-background-color: rgb(157, 2, 8);\n"
"border-color: rgb(246, 211, 45);\n"
"color: rgb(0, 0, 0);"));
        registryDisplayerFrame = new QFrame(startWidget);
        registryDisplayerFrame->setObjectName(QString::fromUtf8("registryDisplayerFrame"));
        registryDisplayerFrame->setGeometry(QRect(0, 0, 800, 480));
        sizePolicy.setHeightForWidth(registryDisplayerFrame->sizePolicy().hasHeightForWidth());
        registryDisplayerFrame->setSizePolicy(sizePolicy);
        registryDisplayerFrame->setMinimumSize(QSize(800, 480));
        registryDisplayerFrame->setMaximumSize(QSize(800, 480));
        registryDisplayerFrame->setFrameShape(QFrame::StyledPanel);
        registryDisplayerFrame->setFrameShadow(QFrame::Raised);
        formLayoutWidget_2 = new QWidget(registryDisplayerFrame);
        formLayoutWidget_2->setObjectName(QString::fromUtf8("formLayoutWidget_2"));
        formLayoutWidget_2->setGeometry(QRect(460, 330, 321, 141));
        formLayout_2 = new QFormLayout(formLayoutWidget_2);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setContentsMargins(0, 0, 0, 0);
        latencyLabel = new QLabel(formLayoutWidget_2);
        latencyLabel->setObjectName(QString::fromUtf8("latencyLabel"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, latencyLabel);

        latencyText = new QLineEdit(formLayoutWidget_2);
        latencyText->setObjectName(QString::fromUtf8("latencyText"));
        latencyText->setReadOnly(true);

        formLayout_2->setWidget(0, QFormLayout::FieldRole, latencyText);

        timeLabel = new QLabel(formLayoutWidget_2);
        timeLabel->setObjectName(QString::fromUtf8("timeLabel"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, timeLabel);

        timeText = new QLineEdit(formLayoutWidget_2);
        timeText->setObjectName(QString::fromUtf8("timeText"));
        timeText->setReadOnly(true);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, timeText);

        formLayoutWidget_3 = new QWidget(registryDisplayerFrame);
        formLayoutWidget_3->setObjectName(QString::fromUtf8("formLayoutWidget_3"));
        formLayoutWidget_3->setGeometry(QRect(20, 330, 331, 141));
        formLayout_3 = new QFormLayout(formLayoutWidget_3);
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        formLayout_3->setContentsMargins(0, 0, 0, 0);
        nameLabel = new QLabel(formLayoutWidget_3);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, nameLabel);

        nameText = new QLineEdit(formLayoutWidget_3);
        nameText->setObjectName(QString::fromUtf8("nameText"));
        nameText->setMouseTracking(true);
        nameText->setAcceptDrops(true);
        nameText->setReadOnly(true);

        formLayout_3->setWidget(0, QFormLayout::FieldRole, nameText);

        dateTimeEdit = new QDateTimeEdit(formLayoutWidget_3);
        dateTimeEdit->setObjectName(QString::fromUtf8("dateTimeEdit"));
        dateTimeEdit->setReadOnly(true);

        formLayout_3->setWidget(1, QFormLayout::FieldRole, dateTimeEdit);

        dateLabel = new QLabel(formLayoutWidget_3);
        dateLabel->setObjectName(QString::fromUtf8("dateLabel"));

        formLayout_3->setWidget(1, QFormLayout::LabelRole, dateLabel);

        plotMeasures = new QCustomPlot(registryDisplayerFrame);
        plotMeasures->setObjectName(QString::fromUtf8("plotMeasures"));
        plotMeasures->setGeometry(QRect(110, 20, 681, 291));
        sizePolicy.setHeightForWidth(plotMeasures->sizePolicy().hasHeightForWidth());
        plotMeasures->setSizePolicy(sizePolicy);
        plotMeasures->setMinimumSize(QSize(681, 291));
        plotMeasures->setMaximumSize(QSize(681, 291));
        startMeasureFrame = new QFrame(startWidget);
        startMeasureFrame->setObjectName(QString::fromUtf8("startMeasureFrame"));
        startMeasureFrame->setGeometry(QRect(0, 0, 800, 480));
        sizePolicy.setHeightForWidth(startMeasureFrame->sizePolicy().hasHeightForWidth());
        startMeasureFrame->setSizePolicy(sizePolicy);
        startMeasureFrame->setMinimumSize(QSize(800, 480));
        startMeasureFrame->setMaximumSize(QSize(800, 480));
        startMeasureFrame->setFrameShape(QFrame::StyledPanel);
        startMeasureFrame->setFrameShadow(QFrame::Raised);
        verticalLayoutWidget_4 = new QWidget(startMeasureFrame);
        verticalLayoutWidget_4->setObjectName(QString::fromUtf8("verticalLayoutWidget_4"));
        verticalLayoutWidget_4->setGeometry(QRect(30, 290, 302, 181));
        startMeasureVerticalLayoutLeft = new QVBoxLayout(verticalLayoutWidget_4);
        startMeasureVerticalLayoutLeft->setObjectName(QString::fromUtf8("startMeasureVerticalLayoutLeft"));
        startMeasureVerticalLayoutLeft->setContentsMargins(0, 0, 0, 0);
        timeFactorLabel = new QLabel(verticalLayoutWidget_4);
        timeFactorLabel->setObjectName(QString::fromUtf8("timeFactorLabel"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(timeFactorLabel->sizePolicy().hasHeightForWidth());
        timeFactorLabel->setSizePolicy(sizePolicy1);
        timeFactorLabel->setMinimumSize(QSize(209, 17));
        timeFactorLabel->setAlignment(Qt::AlignCenter);
        timeFactorLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

        startMeasureVerticalLayoutLeft->addWidget(timeFactorLabel);

        TimeFactorSlider = new QSlider(verticalLayoutWidget_4);
        TimeFactorSlider->setObjectName(QString::fromUtf8("TimeFactorSlider"));
        TimeFactorSlider->setMinimumSize(QSize(300, 0));
        TimeFactorSlider->setMaximumSize(QSize(300, 16777215));
        TimeFactorSlider->setMinimum(100);
        TimeFactorSlider->setMaximum(2000);
        TimeFactorSlider->setOrientation(Qt::Horizontal);
        TimeFactorSlider->setTickPosition(QSlider::TicksBothSides);
        TimeFactorSlider->setTickInterval(500);

        startMeasureVerticalLayoutLeft->addWidget(TimeFactorSlider);

        verticalLayoutWidget_5 = new QWidget(startMeasureFrame);
        verticalLayoutWidget_5->setObjectName(QString::fromUtf8("verticalLayoutWidget_5"));
        verticalLayoutWidget_5->setGeometry(QRect(490, 290, 302, 181));
        startMeasureVerticalLayoutRight = new QVBoxLayout(verticalLayoutWidget_5);
        startMeasureVerticalLayoutRight->setObjectName(QString::fromUtf8("startMeasureVerticalLayoutRight"));
        startMeasureVerticalLayoutRight->setContentsMargins(0, 0, 0, 0);
        DurationLabel = new QLabel(verticalLayoutWidget_5);
        DurationLabel->setObjectName(QString::fromUtf8("DurationLabel"));
        sizePolicy.setHeightForWidth(DurationLabel->sizePolicy().hasHeightForWidth());
        DurationLabel->setSizePolicy(sizePolicy);
        DurationLabel->setBaseSize(QSize(0, 0));
        DurationLabel->setAlignment(Qt::AlignCenter);

        startMeasureVerticalLayoutRight->addWidget(DurationLabel, 0, Qt::AlignHCenter);

        DurationSlider = new QSlider(verticalLayoutWidget_5);
        DurationSlider->setObjectName(QString::fromUtf8("DurationSlider"));
        sizePolicy.setHeightForWidth(DurationSlider->sizePolicy().hasHeightForWidth());
        DurationSlider->setSizePolicy(sizePolicy);
        DurationSlider->setMinimumSize(QSize(300, 0));
        DurationSlider->setMaximumSize(QSize(300, 16777215));
        DurationSlider->setLayoutDirection(Qt::LeftToRight);
        DurationSlider->setMinimum(10);
        DurationSlider->setMaximum(600);
        DurationSlider->setPageStep(5);
        DurationSlider->setValue(10);
        DurationSlider->setSliderPosition(10);
        DurationSlider->setOrientation(Qt::Horizontal);
        DurationSlider->setInvertedAppearance(true);
        DurationSlider->setInvertedControls(false);
        DurationSlider->setTickPosition(QSlider::TicksBothSides);
        DurationSlider->setTickInterval(10);

        startMeasureVerticalLayoutRight->addWidget(DurationSlider);

        calibrateButton = new QPushButton(startMeasureFrame);
        calibrateButton->setObjectName(QString::fromUtf8("calibrateButton"));
        calibrateButton->setGeometry(QRect(370, 320, 80, 25));
        startMeasureButton = new QPushButton(startMeasureFrame);
        startMeasureButton->setObjectName(QString::fromUtf8("startMeasureButton"));
        startMeasureButton->setGeometry(QRect(370, 380, 80, 25));
        sizePolicy.setHeightForWidth(startMeasureButton->sizePolicy().hasHeightForWidth());
        startMeasureButton->setSizePolicy(sizePolicy);
        stopMeasureButton = new QPushButton(startMeasureFrame);
        stopMeasureButton->setObjectName(QString::fromUtf8("stopMeasureButton"));
        stopMeasureButton->setGeometry(QRect(370, 440, 80, 25));
        StartScreen->setCentralWidget(startWidget);
        startMeasureFrame->raise();
        startFrame->raise();
        registryFrame->raise();
        registryDisplayerFrame->raise();
        helpFrame->raise();
        generalInfoFrame->raise();
        settingsFrame->raise();
        backButton->raise();

        retranslateUi(StartScreen);

        QMetaObject::connectSlotsByName(StartScreen);
    } // setupUi

    void retranslateUi(QMainWindow *StartScreen)
    {
        StartScreen->setWindowTitle(QCoreApplication::translate("StartScreen", "Medidor de latencias", nullptr));
        usersManualButton->setText(QCoreApplication::translate("StartScreen", "Manual de usuario", nullptr));
        generalInfoButton->setText(QCoreApplication::translate("StartScreen", "Informaci\303\263n general", nullptr));
        generalInfoText->setPlainText(QCoreApplication::translate("StartScreen", "Medidor de lantecias para sistemas de videovigilancia cr\303\255ticos.\n"
"\n"
"Versi\303\263n de HW: 1.0\n"
"Versi\303\263n de SW: 1.0\n"
"Autor: Guillermo Gir\303\263n Garc\303\255a\n"
"GitHub: @guille31794\n"
"\n"
"Todos los derechos reservados.", nullptr));
        languageLabel->setText(QCoreApplication::translate("StartScreen", "Idioma", nullptr));
        languagesComboBox->setItemText(0, QCoreApplication::translate("StartScreen", "Espa\303\261ol", nullptr));
        languagesComboBox->setItemText(1, QCoreApplication::translate("StartScreen", "English", nullptr));
        languagesComboBox->setItemText(2, QCoreApplication::translate("StartScreen", "Polski", nullptr));

        fontSizeLabel->setText(QCoreApplication::translate("StartScreen", "Tama\303\261o de la fuente", nullptr));
        daltonicCheckbox->setText(QCoreApplication::translate("StartScreen", "Modo dalt\303\263nicos", nullptr));
        registryButtonsBox->setTitle(QString());
        checkRegistryEntryButton->setText(QCoreApplication::translate("StartScreen", "Comprobar", nullptr));
        deleteRegistryEntryButton->setText(QCoreApplication::translate("StartScreen", "Borrar", nullptr));
        renameRegistryEntryButton->setText(QCoreApplication::translate("StartScreen", "Renombrar", nullptr));
        startMeasuringButton->setText(QCoreApplication::translate("StartScreen", "Comenzar medici\303\263n", nullptr));
        historicOfMeasuresButton->setText(QCoreApplication::translate("StartScreen", "Historial de mediciones", nullptr));
        settingsButton->setText(QCoreApplication::translate("StartScreen", "Ajustes", nullptr));
        helpButton->setText(QCoreApplication::translate("StartScreen", "Ayuda", nullptr));
        backButton->setText(QString());
        latencyLabel->setText(QCoreApplication::translate("StartScreen", "Latencia (ms):", nullptr));
        timeLabel->setText(QCoreApplication::translate("StartScreen", "Intervalo (ms):", nullptr));
        nameLabel->setText(QCoreApplication::translate("StartScreen", "Nombre:", nullptr));
        dateLabel->setText(QCoreApplication::translate("StartScreen", "Fecha:", nullptr));
        timeFactorLabel->setText(QCoreApplication::translate("StartScreen", "Int\303\251rvalo entre mediciones (ms)", nullptr));
        DurationLabel->setText(QCoreApplication::translate("StartScreen", "Duraci\303\263n (s)", nullptr));
        calibrateButton->setText(QCoreApplication::translate("StartScreen", "Calibrar", nullptr));
        startMeasureButton->setText(QCoreApplication::translate("StartScreen", "Comenzar", nullptr));
        stopMeasureButton->setText(QCoreApplication::translate("StartScreen", "Detener", nullptr));
    } // retranslateUi

};

namespace Ui {
    class StartScreen: public Ui_StartScreen {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STARTSCREEN_H

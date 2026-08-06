#include "mainwindow.h"
#include "appsettings.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mStackedWidget(new QStackedWidget(this))
    , mHomeScreen(new HomeScreen(this))
    , mSettingsScreen(new SettingsScreen(this))
    , mHelpScreen(new HelpScreen(this))
    , mHelpInfoScreen(new HelpInfoScreen(this))
    , mRegistryScreen(new RegistryScreen(this))
    , mRegistryDisplayScreen(new RegistryDisplayScreen(this))
    , mStartScreen(new StartScreen(this))
{
    // Load settings FIRST (before any UI painting)
    AppSettings::instance().load();

    setCentralWidget(mStackedWidget);

    // Add screens to the stack
    mStackedWidget->addWidget(mHomeScreen);      // Index 0: Home
    mStackedWidget->addWidget(mSettingsScreen);   // Index 1: Settings
    mStackedWidget->addWidget(mHelpScreen);       // Index 2: Help menu
    mStackedWidget->addWidget(mHelpInfoScreen);   // Index 3: Help info display
    mStackedWidget->addWidget(mRegistryScreen);   // Index 4: Registry
    mStackedWidget->addWidget(mRegistryDisplayScreen); // Index 5: Registry display
    mStackedWidget->addWidget(mStartScreen);      // Index 6: Legacy (measure only)

    // Apply theme from settings to the top-level window before showing
    this->setStyleSheet(AppSettings::instance().currentStylesheet());

    // React to any settings change: re-apply theme and font globally
    connect(&AppSettings::instance(), &AppSettings::settingsApplied, this, [this]() {
        this->setStyleSheet(AppSettings::instance().currentStylesheet());

        // Apply font size to all children except backButtons
        QFont font;
        font.setPointSize(AppSettings::instance().fontSize());
        const auto children = this->findChildren<QWidget*>();
        for (QWidget* child : children)
        {
            if (child->objectName() != "backButton")
            {
                child->setFont(font);
            }
        }

        // Apply daltonic mode: set global daltonic background after theme
        if (AppSettings::instance().daltonicMode())
        {
            QString daltonicSheet = AppSettings::instance().currentStylesheet()
                + " * { background-color: rgb(82, 183, 136); }";
            // Keep back buttons untouched (they have inline style with higher specificity)
            this->setStyleSheet(daltonicSheet);
        }
    });

    // Also react to theme-only changes (e.g. at startup)
    connect(&AppSettings::instance(), &AppSettings::themeChanged, this, [this](const QString& stylesheet) {
        this->setStyleSheet(stylesheet);
    });

    // Start on home screen
    mStackedWidget->setCurrentIndex(HOME);

    // Connect HomeScreen navigation signals
    connect(mHomeScreen, &HomeScreen::startMeasuringRequested, this, &MainWindow::showMeasureScreen);
    connect(mHomeScreen, &HomeScreen::historyRequested, this, &MainWindow::showHistoryScreen);
    connect(mHomeScreen, &HomeScreen::settingsRequested, this, &MainWindow::showSettingsScreen);
    connect(mHomeScreen, &HomeScreen::helpRequested, this, &MainWindow::showHelpScreen);

    // Connect SettingsScreen back signal
    connect(mSettingsScreen, &SettingsScreen::backRequested, this, &MainWindow::showHomeScreen);

    // Connect HelpScreen back signal
    connect(mHelpScreen, &HelpScreen::backRequested, this, &MainWindow::showHomeScreen);

    // Connect HelpScreen navigation to info screen
    connect(mHelpScreen, &HelpScreen::generalInfoRequested, this, [this]() {
        mHelpInfoScreen->setContent(tr(
            "Medidor de latencias para sistemas de videovigilancia críticos.\n\n"
            "Versión de HW: 1.0\n"
            "Versión de SW: 1.0\n"
            "Autor: Guillermo Girón García\n"
            "GitHub: @guille31794\n\n"
            "Todos los derechos reservados."));
        mStackedWidget->setCurrentIndex(HELP_INFO);
    });
    connect(mHelpScreen, &HelpScreen::usersManualRequested, this, [this]() {
        mHelpInfoScreen->setContent(tr("Manual de usuario en desarrollo..."));
        mStackedWidget->setCurrentIndex(HELP_INFO);
    });

    // Connect HelpInfoScreen back to help menu
    connect(mHelpInfoScreen, &HelpInfoScreen::backRequested, this, [this]() {
        mStackedWidget->setCurrentIndex(HELP);
    });

    // Connect RegistryScreen
    connect(mRegistryScreen, &RegistryScreen::backRequested, this, &MainWindow::showHomeScreen);
    connect(mRegistryScreen, &RegistryScreen::measureSelected, this, [this](const Measures& measure) {
        mRegistryDisplayScreen->displayMeasure(measure);
        mStackedWidget->setCurrentIndex(REGISTRY_DISPLAY);
    });

    // Connect RegistryDisplayScreen back to registry
    connect(mRegistryDisplayScreen, &RegistryDisplayScreen::backRequested, this, [this]() {
        mStackedWidget->setCurrentIndex(REGISTRY);
    });

    // Connect StartScreen back-to-home signal
    connect(mStartScreen, &StartScreen::backToHome, this, &MainWindow::showHomeScreen);

    // Set window size for the target display
    setFixedSize(800, 480);

    // Apply initial font size to all children (except back buttons)
    QFont initialFont;
    initialFont.setPointSize(AppSettings::instance().fontSize());
    const auto allChildren = this->findChildren<QWidget*>();
    for (QWidget* child : allChildren)
    {
        if (child->objectName() != "backButton")
        {
            child->setFont(initialFont);
        }
    }

    // Apply initial daltonic mode
    if (AppSettings::instance().daltonicMode())
    {
        QString daltonicSheet = AppSettings::instance().currentStylesheet()
            + " * { background-color: rgb(82, 183, 136); }";
        this->setStyleSheet(daltonicSheet);
    }
}

void MainWindow::showMeasureScreen()
{
    mStackedWidget->setCurrentIndex(LEGACY);
    mStartScreen->navigateTo(MenuScreen::START_MEASURE_SCREEN);
}

void MainWindow::showHistoryScreen()
{
    mStackedWidget->setCurrentIndex(REGISTRY);
}

void MainWindow::showSettingsScreen()
{
    mStackedWidget->setCurrentIndex(SETTINGS);
}

void MainWindow::showHelpScreen()
{
    mStackedWidget->setCurrentIndex(HELP);
}

void MainWindow::showHomeScreen()
{
    mStackedWidget->setCurrentIndex(HOME);
}

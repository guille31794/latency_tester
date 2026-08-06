#include "appsettings.h"

AppSettings& AppSettings::instance()
{
    static AppSettings instance;
    return instance;
}

AppSettings::AppSettings(QObject *parent)
    : QObject(parent)
{
    initStylesheets();
}

void AppSettings::initStylesheets()
{
    // Light theme
    mThemeStylesheets.insert(false,
        "QPushButton {"
        "  border: 2px solid rgba(255, 255, 255, 180);"
        "  border-radius: 8px;"
        "  padding: 6px 16px;"
        "  font-weight: bold;"
        "  color: white;"
        "  background-color: rgba(0, 120, 180, 200);"
        "}"
        "QPushButton:hover { background-color: rgba(0, 150, 220, 220); border-color: rgba(255, 255, 255, 240); }"
        "QPushButton:pressed { background-color: rgba(0, 90, 140, 240); }"
        "QPushButton:disabled {"
        "  background-color: rgba(100, 100, 100, 150);"
        "  border-color: rgba(150, 150, 150, 100);"
        "  color: rgba(200, 200, 200, 150);"
        "}"
        "QSlider::groove:horizontal {"
        "  border: 1px solid rgb(160, 160, 180);"
        "  height: 8px;"
        "  background: rgb(200, 210, 230);"
        "  border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "  background: rgb(0, 140, 200);"
        "  border: 1px solid rgb(0, 110, 170);"
        "  width: 18px;"
        "  margin: -6px 0;"
        "  border-radius: 9px;"
        "}"
    );

    // Dark theme
    mThemeStylesheets.insert(true,
        "* { background-color: rgb(35, 35, 50); color: rgb(210, 210, 220); }"
        "QPushButton {"
        "  border: 2px solid rgba(180, 180, 200, 150);"
        "  border-radius: 8px;"
        "  padding: 6px 16px;"
        "  font-weight: bold;"
        "  color: white;"
        "  background-color: rgba(40, 80, 120, 220);"
        "}"
        "QPushButton:hover { background-color: rgba(50, 100, 150, 230); }"
        "QPushButton:pressed { background-color: rgba(30, 60, 90, 240); }"
        "QPushButton:disabled {"
        "  background-color: rgba(60, 60, 60, 150);"
        "  color: rgba(150, 150, 150, 150);"
        "}"
        "QSlider::groove:horizontal {"
        "  border: 1px solid rgb(80, 80, 100);"
        "  height: 8px;"
        "  background: rgb(60, 60, 80);"
        "  border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "  background: rgb(100, 160, 220);"
        "  border: 1px solid rgb(70, 130, 190);"
        "  width: 18px;"
        "  margin: -6px 0;"
        "  border-radius: 9px;"
        "}"
        "QComboBox {"
        "  background-color: rgb(50, 50, 65);"
        "  border: 1px solid rgb(80, 80, 100);"
        "  border-radius: 4px;"
        "  padding: 4px;"
        "  color: rgb(210, 210, 220);"
        "}"
        "QComboBox::drop-down { border: none; }"
        "QComboBox QAbstractItemView { background-color: rgb(50, 50, 65); color: rgb(210, 210, 220); }"
        "QCheckBox { spacing: 8px; }"
        "QCheckBox::indicator { width: 18px; height: 18px; }"
        "QCheckBox::indicator:unchecked { border: 2px solid rgb(120, 120, 140); border-radius: 3px; background: rgb(50, 50, 65); }"
        "QCheckBox::indicator:checked { border: 2px solid rgb(100, 160, 220); border-radius: 3px; background: rgb(100, 160, 220); }"
        "QGroupBox { border: 1px solid rgb(80, 80, 100); border-radius: 6px; margin-top: 8px; }"
        "QGroupBox::title { color: rgb(180, 180, 200); }"
        "QTreeView { background-color: rgb(40, 40, 55); color: rgb(200, 200, 210); border: 1px solid rgb(70, 70, 90); }"
        "QLabel { background-color: transparent; }"
    );

    // Daltonic widget colors
    mDaltonicWidgetStyles.insert(true, "background-color: rgb(82, 183, 136)");
    mDaltonicWidgetStyles.insert(false, "");
    mDaltonicBackButtonStyles.insert(true, "background-color: rgb(157, 2, 8)");
    mDaltonicBackButtonStyles.insert(false, "");
}

void AppSettings::load()
{
    QSettings settings{ORG_NAME, APP_NAME};

    if (settings.value("Language") != 0)
    {
        mSettings.language = settings.value("Language").value<Languages>();
    }

    if (settings.value("FontSize") != 0)
    {
        mSettings.fontSize = settings.value("FontSize").value<quint8>();
    }

    if (mSettings.fontSize == 0)
    {
        mSettings.fontSize = 9;
    }

    mSettings.daltonicMode = settings.value("DaltonicMode").value<bool>();
    mSettings.darkMode = settings.value("DarkMode").value<bool>();
}

void AppSettings::save()
{
    QSettings settings{ORG_NAME, APP_NAME};
    settings.setValue("Language", static_cast<quint8>(mSettings.language));
    settings.setValue("FontSize", mSettings.fontSize);
    settings.setValue("DaltonicMode", mSettings.daltonicMode);
    settings.setValue("DarkMode", mSettings.darkMode);
}

QString AppSettings::currentStylesheet() const
{
    return mThemeStylesheets.value(mSettings.darkMode);
}

QString AppSettings::daltonicWidgetStyle() const
{
    return mDaltonicWidgetStyles.value(mSettings.daltonicMode);
}

QString AppSettings::daltonicBackButtonStyle() const
{
    return mDaltonicBackButtonStyles.value(mSettings.daltonicMode);
}

void AppSettings::setLanguage(Languages lang)
{
    mSettings.language = lang;
    emit languageChanged(lang);
}

void AppSettings::setFontSize(quint8 size)
{
    mSettings.fontSize = size;
    emit fontSizeChanged(size);
}

void AppSettings::setDarkMode(bool enabled)
{
    mSettings.darkMode = enabled;
    emit themeChanged(currentStylesheet());
}

void AppSettings::setDaltonicMode(bool enabled)
{
    mSettings.daltonicMode = enabled;
    emit daltonicModeChanged(enabled);
}

void AppSettings::applySettings(const GeneralConfigSettings& settings)
{
    bool themeNeedsUpdate = (settings.darkMode != mSettings.darkMode);
    bool fontNeedsUpdate = (settings.fontSize != mSettings.fontSize);
    bool langNeedsUpdate = (settings.language != mSettings.language);
    bool daltonicNeedsUpdate = (settings.daltonicMode != mSettings.daltonicMode);

    mSettings = settings;
    save();

    if (themeNeedsUpdate)
        emit themeChanged(currentStylesheet());
    if (fontNeedsUpdate)
        emit fontSizeChanged(mSettings.fontSize);
    if (langNeedsUpdate)
        emit languageChanged(mSettings.language);
    if (daltonicNeedsUpdate)
        emit daltonicModeChanged(mSettings.daltonicMode);

    emit settingsApplied();
}

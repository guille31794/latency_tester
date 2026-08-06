#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include "dataModel.hpp"
#include <QObject>
#include <QSettings>
#include <QString>
#include <QMap>

/**
 * @brief The AppSettings class manages application-wide settings (CORE layer).
 * It loads/saves settings from QSettings and provides stylesheets and configuration
 * to the GUI layer. Emits signals when settings change so UI can react.
 */
class AppSettings : public QObject
{
    Q_OBJECT

public:
    static AppSettings& instance();

    void load();
    void save();

    // Getters
    GeneralConfigSettings current() const { return mSettings; }
    Languages language() const { return mSettings.language; }
    quint8 fontSize() const { return mSettings.fontSize; }
    bool darkMode() const { return mSettings.darkMode; }
    bool daltonicMode() const { return mSettings.daltonicMode; }
    QString currentStylesheet() const;
    QString daltonicWidgetStyle() const;
    QString daltonicBackButtonStyle() const;

    // Setters (emit signals)
    void setLanguage(Languages lang);
    void setFontSize(quint8 size);
    void setDarkMode(bool enabled);
    void setDaltonicMode(bool enabled);
    void applySettings(const GeneralConfigSettings& settings);

signals:
    void themeChanged(const QString& stylesheet);
    void fontSizeChanged(quint8 size);
    void languageChanged(Languages lang);
    void daltonicModeChanged(bool enabled);
    void settingsApplied();

private:
    AppSettings(QObject *parent = nullptr);
    void initStylesheets();

    GeneralConfigSettings mSettings;
    QMap<bool, QString> mThemeStylesheets;  // false=light, true=dark
    QMap<bool, QString> mDaltonicWidgetStyles;
    QMap<bool, QString> mDaltonicBackButtonStyles;

    static constexpr const char* ORG_NAME = "TFG Guillermo Giron Garcia";
    static constexpr const char* APP_NAME = "Latency Tester";
};

#endif // APPSETTINGS_H

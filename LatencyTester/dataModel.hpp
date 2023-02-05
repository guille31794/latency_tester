#ifndef DATAMODEL_HPP
#define DATAMODEL_HPP

#include <QString>
#include <QVector>
#include <QMap>
#include <QMetaType>
#include <QDate>

enum class MenuScreen
{
    START_SCREEN,
    START_MEASURE_SCREEN,
    MEASURES_REGISTRY_SCREEN,
    SETTINGS_SCREEN,
    HELP_SCREEN,
    GENERAL_INFO_SCREEN,
    USERS_MANUAL_SCREEN,
    REGISTRY_DISPLAYER_SCREEN
};

enum class Languages
{
    SPANISH = 0,
    ENGLISH,
    POLISH
};

struct GeneralConfigSettings
{
    Languages language{Languages::SPANISH};
    quint8 fontSize{10};
    bool daltonicMode{false};
};

struct Colors
{
    QString backButton;
    QString widgets;
};

struct Measures
{
    QString name;
    QDateTime date;
    int timeFactor;
    int duration;
    QVector<double> lantencies{};
    double meanLatency{0};
};

Q_DECLARE_METATYPE(Languages);

#endif // DATAMODEL_HPP

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
    USERS_MANUAL_SCREEN
};

enum class Languages
{
    SPANISH = 0,
    ENGLISH,
    POLISH
};

struct GeneralConfigSettings
{
    Languages language = Languages::SPANISH;
    quint8 fontSize = 10;
};

struct Measures
{
    QString name;
    QDate date;
    QVector<uint> time;
    QVector<uint> latencys;
};

Q_DECLARE_METATYPE(Languages);

#endif // DATAMODEL_HPP

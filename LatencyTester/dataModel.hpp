#ifndef DATAMODEL_HPP
#define DATAMODEL_HPP

#include <QString>
#include <QVector>
#include <QMap>
#include <QMetaType>

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
    SPANISH = 1,
    ENGLISH,
    POLISH
};

struct GeneralConfigSettings
{
    Languages language = Languages::SPANISH;
    quint8 fontSize = 10;
};

Q_DECLARE_METATYPE(Languages);

#endif // DATAMODEL_HPP

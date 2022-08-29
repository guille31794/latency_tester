#ifndef DATAMODEL_HPP
#define DATAMODEL_HPP

#include <QString>
#include <QVector>
#include <QMap>

enum class MenuScreen
{
    START_SCREEN,
    START_MEASURE_SCREEN,
    MEASURES_REGISTRY_SCREEN,
    SETTINGS_SCREEN,
    HELP_SCREEN,
    GENERAL_INFO_SCREEN,
    USERS_MANUAL_SCREEN,
};

enum class Languages
{
    SPANISH,
    ENGLISH,
    POLISH
};

struct GeneralConfigSettings
{
    Languages language;
    bool daltonicMode;
    quint8 fontSize = 12;
};



#endif // DATAMODEL_HPP

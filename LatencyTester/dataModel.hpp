#ifndef DATAMODEL_HPP
#define DATAMODEL_HPP

#include <QString>
#include <QVector>
#include <QMap>
#include <iostream>

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
    bool daltonicMode = false;
    quint8 fontSize = 10;
};

#endif // DATAMODEL_HPP

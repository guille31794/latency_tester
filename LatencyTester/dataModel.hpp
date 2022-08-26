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
    USER_MANUAL_SCREEN,
};

struct GeneralConfig
{
    QString language;
    bool daltonicMode;
    quint8 fontSize;
};


#endif // DATAMODEL_HPP

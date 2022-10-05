#ifndef JSONOPERATOR_H
#define JSONOPERATOR_H

#include "dataModel.hpp"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QPointer>
#include <QJsonValue>

class JsonOperator
{
public:
    JsonOperator() = default;
    bool loadFileFromDisk(const QString& fileName, QIODevice::OpenModeFlag openMode = QIODevice::ReadOnly);
    bool saveMeasureToDisk();
    void parseJsonToStruct(Measures& registry);
private:
    QJsonDocument mJsonDocument;
    QJsonObject mJsonObject;
    QPointer<QFile> mFile;
};

#endif // JSONOPERATOR_H

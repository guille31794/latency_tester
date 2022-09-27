#ifndef JSONOPERATOR_H
#define JSONOPERATOR_H

#include "dataModel.hpp"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QPointer>

class JsonOperator
{
public:
    JsonOperator();
    bool loadFileFromDisk(const QString& fileName, QIODevice::OpenModeFlag openMode = QIODevice::ReadOnly);

private:
    QPointer<QJsonDocument> mJsonDocument;
    QPointer<QFile> mFile;
};

#endif // JSONOPERATOR_H

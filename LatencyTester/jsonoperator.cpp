#include "jsonoperator.h"

const QString NAME{"Name"};
const QString DATE{"Date"};
const QString TIMEFACTOR{"TimeFactor"};
const QString LATENCYS{"Latencys"};

bool JsonOperator::loadFileFromDisk(const QString &fileName, QIODevice::OpenModeFlag openMode)
{
    bool loadSuccess{true};
    mFile = new QFile(fileName);

    if(!mFile->exists())
    {
        loadSuccess = false;
    }

    if(loadSuccess && mFile->open(openMode))
    {
        QByteArray bytesFile{mFile->readAll()};
        mJsonDocument = QJsonDocument(QJsonDocument::fromJson(bytesFile));
        mJsonObject = mJsonDocument.object();
        mFile->close();
    }
    else
    {
        loadSuccess = false;
    }

    return loadSuccess;
}

bool JsonOperator::saveMeasureToDisk()
{
    bool saveSuccess{true};

    return saveSuccess;
}

void JsonOperator::parseJsonToStruct(Measures &registry)
{
    registry.name = mJsonObject.value(NAME).toString();
    registry.date = QDateTime::fromString(mJsonObject.value(DATE).toString(), Qt::ISODate);

    QJsonArray timeFactors{mJsonObject.value(TIMEFACTOR).toArray()};
    for(auto timeFactor : timeFactors)
    {
        registry.times.append(timeFactor.toInt());
    }

    QJsonArray latencys{mJsonObject.value(LATENCYS).toArray()};
    for(auto latency : latencys)
    {
        registry.latencys.append(latency.toInt());
    }
}

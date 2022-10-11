#include "jsonoperator.h"

#include <QDir>

const QString NAME{"Name"};
const QString DATE{"Date"};
const QString TIMEFACTOR{"TimeFactor"};
const QString LATENCYS{"Latencys"};
const QString DURATION{"Duration"};
const QString MEASURE{"_Medicion.json"};

JsonOperator::JsonOperator(const QString &path): mPath{path}
{}

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

bool JsonOperator::saveMeasureToDisk(const Measures &registry)
{
    bool saveSuccess{true};

    parseStructToJson(registry);

    mFile = new QFile(mPath + registry.date.toString(Qt::ISODate) + MEASURE);

    if(!mFile->open(QIODevice::OpenModeFlag::WriteOnly))
    {
        mFile->write(mJsonDocument.toJson());
        mFile->close();
    }
    else
    {
        saveSuccess = false;
    }

    return saveSuccess;
}

void JsonOperator::parseJsonToStruct(Measures &registry)
{
    registry.name = mJsonObject.value(NAME).toString();
    registry.date = QDateTime::fromString(mJsonObject.value(DATE).toString(), Qt::ISODate);
    registry.duration = mJsonObject.value(DURATION).toInt();
    registry.timeFactor = mJsonObject.value(TIMEFACTOR).toInt();
    registry.latencys.clear();
    registry.meanLatency = 0;

    for(int index = 0; index < mJsonObject.value(LATENCYS).toArray().size(); ++index)
    {
        registry.latencys.append(mJsonObject.value(LATENCYS).toArray().at(index).toDouble());
        registry.meanLatency += mJsonObject.value(LATENCYS).toArray().at(index).toDouble();
    }

    registry.meanLatency /= registry.latencys.size();
}

void JsonOperator::setPath(const QString &path)
{
    mPath = path;
}

void JsonOperator::parseStructToJson(const Measures &registry)
{
    QJsonArray array;
    mJsonObject = QJsonObject();
    mJsonObject.insert(NAME, registry.name);
    mJsonObject.insert(DATE, registry.date.toString(Qt::ISODate));
    mJsonObject.insert(TIMEFACTOR, registry.timeFactor);
    mJsonObject.insert(DURATION, registry.duration);

    for(auto& latency : registry.latencys)
    {
        array.append(latency);
    }

    mJsonObject.insert(LATENCYS, array);
    mJsonDocument = QJsonDocument(mJsonObject);
}

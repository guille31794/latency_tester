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

    for(int index = 0; index < mJsonObject.value(TIMEFACTOR).toArray().size(); ++index)
    {
        registry.times.append(mJsonObject.value(TIMEFACTOR).toArray().at(index).toDouble());
        registry.meanFactor += mJsonObject.value(TIMEFACTOR).toArray().at(index).toDouble();
    }

    registry.meanFactor /= registry.times.size();

    for(int index = 0; index < mJsonObject.value(LATENCYS).toArray().size(); ++index)
    {
        registry.latencys.append(mJsonObject.value(LATENCYS).toArray().at(index).toDouble());
        registry.meanLatency += mJsonObject.value(LATENCYS).toArray().at(index).toDouble();
    }

    registry.meanLatency /= registry.latencys.size();
}

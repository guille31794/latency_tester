#include "jsonoperator.h"

bool JsonOperator::loadFileFromDisk(const QString &fileName, QIODevice::OpenModeFlag openMode)
{
    bool loadSucces{true};
    mFile = new QFile(fileName);

    if(!mFile->exists())
    {
        loadSucces = false;
    }

    if(loadSucces && mFile->open(openMode))
    {
        QByteArray bytesFile{mFile->readAll()};
        mJsonDocument = QJsonDocument(QJsonDocument::fromJson(bytesFile));
        mJsonObject = mJsonDocument.object();
        mFile->close();
    }
    else
    {
        qDebug() << "Fallo al leer el fichero Json";
        loadSucces = false;
    }

    return loadSucces;
}

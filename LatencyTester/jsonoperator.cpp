#include "jsonoperator.h"

JsonOperator::JsonOperator(): mJsonDocument{nullptr}, mFile{nullptr}
{}

bool JsonOperator::loadFileFromDisk(const QString &fileName, QIODevice::OpenModeFlag openMode)
{
    bool loadSucces{true};
    mFile = new QFile(fileName);

    if(mFile->open(openMode))
    {

    }
    else
    {
        qDebug() << "Fallo al leer el fichero Json";
        loadSucces = false;
    }

    return loadSucces;
}

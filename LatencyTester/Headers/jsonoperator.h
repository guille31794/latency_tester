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
    JsonOperator(const QString& path);
    /**
     * @brief loadFileFromDisk load a .json file from disk.
     * @param fileName path of the file to open.
     * @param openMode oppening mode flag.
     * @return true if the file was correctly loaded or false othewise.
     */
    bool loadFileFromDisk(const QString& fileName, QIODevice::OpenModeFlag openMode = QIODevice::ReadOnly);
    /**
     * @brief saveMeasureToDisk save a .json file to disk.
     * @param registry data container to save in the file.
     * @return true if save process was succesful or false otherwise.
     */
    bool saveMeasureToDisk(const Measures& registry);
    /**
     * @brief parseJsonToStruct transform data extracted from a .json file to a measure registry.
     * @param registry memory where store the parsed data.
     */
    void parseJsonToStruct(Measures& registry);
    /**
     * @brief setPath indicates where files has to be saved and loaded from.
     * @param path route to locate files.
     */
    void setPath(const QString& path);

private:
    /**
     * @brief parseStructToJson transform data from a measure registry to .json formatted data file.
     * @param registry container from measure data.
     */
    void parseStructToJson(const Measures& registry);

    QJsonDocument mJsonDocument;
    QJsonObject mJsonObject;
    QPointer<QFile> mFile;
    QString mPath;
};

#endif // JSONOPERATOR_H

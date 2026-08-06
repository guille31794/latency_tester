#ifndef TST_JSONOPERATOR_H
#define TST_JSONOPERATOR_H

#include <QObject>
#include <QTemporaryDir>

class TestJsonOperator : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // loadFileFromDisk
    void test_loadFile_validJsonReturnsTrue();
    void test_loadFile_nonExistentFileReturnsFalse();
    void test_loadFile_emptyFileReturnsTrueButEmptyObject();

    // parseJsonToStruct
    void test_parseToStruct_nameCorrect();
    void test_parseToStruct_dateCorrect();
    void test_parseToStruct_timeFactorCorrect();
    void test_parseToStruct_durationCorrect();
    void test_parseToStruct_latenciesCorrect();
    void test_parseToStruct_meanLatencyCalculated();
    void test_parseToStruct_emptyLatenciesArray();
    void test_parseToStruct_missingFieldsDefaultToZeroOrEmpty();

    // saveMeasureToDisk
    void test_saveMeasure_createsFile();
    void test_saveMeasure_contentIsValidJson();
    void test_saveMeasure_roundTrip();

    // setPath
    void test_setPath_changesOutputDirectory();

private:
    QTemporaryDir *mTempDir{nullptr};
    QString mTestDataPath;
};

#endif // TST_JSONOPERATOR_H

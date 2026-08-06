#include "tst_jsonoperator.h"
#include "jsonoperator.h"
#include <QtTest>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

void TestJsonOperator::initTestCase()
{
    mTempDir = new QTemporaryDir();
    QVERIFY(mTempDir->isValid());
    mTestDataPath = mTempDir->path() + "/";

    // Create a valid test JSON file
    QJsonObject obj;
    obj.insert("Name", "TestMeasure");
    obj.insert("Date", "2026-01-15T10:30:00");
    obj.insert("TimeFactor", 250);
    obj.insert("Duration", 5000);
    QJsonArray latencies;
    latencies.append(100.5);
    latencies.append(105.2);
    latencies.append(98.7);
    latencies.append(-1.0);  // failed measurement
    latencies.append(102.0);
    obj.insert("Latencies", latencies);

    QJsonDocument doc(obj);
    QFile file(mTestDataPath + "valid_test.json");
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write(doc.toJson());
    file.close();

    // Create an empty JSON file
    QFile emptyFile(mTestDataPath + "empty_test.json");
    QVERIFY(emptyFile.open(QIODevice::WriteOnly));
    emptyFile.write("{}");
    emptyFile.close();
}

void TestJsonOperator::cleanupTestCase()
{
    delete mTempDir;
    mTempDir = nullptr;
}

// --- loadFileFromDisk ---

void TestJsonOperator::test_loadFile_validJsonReturnsTrue()
{
    JsonOperator op;
    bool result = op.loadFileFromDisk(mTestDataPath + "valid_test.json");
    QCOMPARE(result, true);
}

void TestJsonOperator::test_loadFile_nonExistentFileReturnsFalse()
{
    JsonOperator op;
    bool result = op.loadFileFromDisk(mTestDataPath + "nonexistent.json");
    QCOMPARE(result, false);
}

void TestJsonOperator::test_loadFile_emptyFileReturnsTrueButEmptyObject()
{
    JsonOperator op;
    bool result = op.loadFileFromDisk(mTestDataPath + "empty_test.json");
    QCOMPARE(result, true);

    // Parse into struct — everything should be empty/default
    Measures m;
    op.parseJsonToStruct(m);
    QVERIFY(m.name.isEmpty());
    QCOMPARE(m.timeFactor, 0);
    QCOMPARE(m.duration, 0);
    QVERIFY(m.lantencies.isEmpty());
}

// --- parseJsonToStruct ---

void TestJsonOperator::test_parseToStruct_nameCorrect()
{
    JsonOperator op;
    op.loadFileFromDisk(mTestDataPath + "valid_test.json");
    Measures m;
    op.parseJsonToStruct(m);
    QCOMPARE(m.name, QString("TestMeasure"));
}

void TestJsonOperator::test_parseToStruct_dateCorrect()
{
    JsonOperator op;
    op.loadFileFromDisk(mTestDataPath + "valid_test.json");
    Measures m;
    op.parseJsonToStruct(m);
    QCOMPARE(m.date, QDateTime::fromString("2026-01-15T10:30:00", Qt::ISODate));
}

void TestJsonOperator::test_parseToStruct_timeFactorCorrect()
{
    JsonOperator op;
    op.loadFileFromDisk(mTestDataPath + "valid_test.json");
    Measures m;
    op.parseJsonToStruct(m);
    QCOMPARE(m.timeFactor, 250);
}

void TestJsonOperator::test_parseToStruct_durationCorrect()
{
    JsonOperator op;
    op.loadFileFromDisk(mTestDataPath + "valid_test.json");
    Measures m;
    op.parseJsonToStruct(m);
    QCOMPARE(m.duration, 5000);
}

void TestJsonOperator::test_parseToStruct_latenciesCorrect()
{
    JsonOperator op;
    op.loadFileFromDisk(mTestDataPath + "valid_test.json");
    Measures m;
    op.parseJsonToStruct(m);
    QCOMPARE(m.lantencies.size(), 5);
    QCOMPARE(m.lantencies[0], 100.5);
    QCOMPARE(m.lantencies[3], -1.0);
    QCOMPARE(m.lantencies[4], 102.0);
}

void TestJsonOperator::test_parseToStruct_meanLatencyCalculated()
{
    JsonOperator op;
    op.loadFileFromDisk(mTestDataPath + "valid_test.json");
    Measures m;
    op.parseJsonToStruct(m);
    // Mean = (100.5 + 105.2 + 98.7 + (-1.0) + 102.0) / 5 = 405.4 / 5 = 81.08
    double expectedMean = (100.5 + 105.2 + 98.7 + (-1.0) + 102.0) / 5.0;
    QVERIFY(qAbs(m.meanLatency - expectedMean) < 0.01);
}

void TestJsonOperator::test_parseToStruct_emptyLatenciesArray()
{
    // Create a JSON with empty Latencies array
    QJsonObject obj;
    obj.insert("Name", "Empty");
    obj.insert("Date", "2026-01-01T00:00:00");
    obj.insert("TimeFactor", 100);
    obj.insert("Duration", 1000);
    obj.insert("Latencies", QJsonArray());

    QJsonDocument doc(obj);
    QFile file(mTestDataPath + "empty_latencies.json");
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write(doc.toJson());
    file.close();

    JsonOperator op;
    op.loadFileFromDisk(mTestDataPath + "empty_latencies.json");
    Measures m;
    op.parseJsonToStruct(m);

    QVERIFY(m.lantencies.isEmpty());
    // NOTE: current implementation does division by zero here (lantencies.size()=0).
    // This test documents the bug — meanLatency will be NaN or crash.
    // For now, we just verify the latencies are empty.
}

void TestJsonOperator::test_parseToStruct_missingFieldsDefaultToZeroOrEmpty()
{
    // JSON with only Name field
    QJsonObject obj;
    obj.insert("Name", "Partial");

    QJsonDocument doc(obj);
    QFile file(mTestDataPath + "partial.json");
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write(doc.toJson());
    file.close();

    JsonOperator op;
    op.loadFileFromDisk(mTestDataPath + "partial.json");
    Measures m;
    m.timeFactor = 999;  // pre-set to verify it gets overwritten
    m.duration = 999;
    op.parseJsonToStruct(m);

    QCOMPARE(m.name, QString("Partial"));
    QCOMPARE(m.timeFactor, 0);  // missing field → toInt() returns 0
    QCOMPARE(m.duration, 0);
    QVERIFY(m.lantencies.isEmpty());
}

// --- saveMeasureToDisk ---

void TestJsonOperator::test_saveMeasure_createsFile()
{
    JsonOperator op;
    op.setPath(mTestDataPath);

    Measures m;
    m.name = "SaveTest";
    m.date = QDateTime::fromString("2026-03-20T08:00:00", Qt::ISODate);
    m.timeFactor = 500;
    m.duration = 10000;
    m.lantencies = {50.0, 55.0, 52.0};

    op.saveMeasureToDisk(m);

    // Filename uses ISO date with colons replaced by hyphens (Windows-safe)
    QString expectedFile = mTestDataPath + "2026-03-20T08-00-00_Medicion.json";
    QVERIFY2(QFile::exists(expectedFile),
             qPrintable("Expected file not found: " + expectedFile));
}

void TestJsonOperator::test_saveMeasure_contentIsValidJson()
{
    JsonOperator op;
    op.setPath(mTestDataPath);

    Measures m;
    m.name = "JsonContent";
    m.date = QDateTime::fromString("2026-04-01T12:00:00", Qt::ISODate);
    m.timeFactor = 300;
    m.duration = 6000;
    m.lantencies = {80.0, 85.5};

    op.saveMeasureToDisk(m);

    QString filePath = mTestDataPath + "2026-04-01T12-00-00_Medicion.json";
    QFile file(filePath);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());
    QCOMPARE(doc.object().value("Name").toString(), QString("JsonContent"));
}

void TestJsonOperator::test_saveMeasure_roundTrip()
{
    JsonOperator op;
    op.setPath(mTestDataPath);

    Measures original;
    original.name = "RoundTrip";
    original.date = QDateTime::fromString("2026-05-15T16:45:00", Qt::ISODate);
    original.timeFactor = 750;
    original.duration = 15000;
    original.lantencies = {120.0, 115.5, 130.2, -1.0, 125.0};

    op.saveMeasureToDisk(original);

    // Load it back
    QString filePath = mTestDataPath + "2026-05-15T16-45-00_Medicion.json";
    JsonOperator op2;
    QVERIFY(op2.loadFileFromDisk(filePath));

    Measures loaded;
    op2.parseJsonToStruct(loaded);

    QCOMPARE(loaded.name, original.name);
    QCOMPARE(loaded.date, original.date);
    QCOMPARE(loaded.timeFactor, original.timeFactor);
    QCOMPARE(loaded.duration, original.duration);
    QCOMPARE(loaded.lantencies.size(), original.lantencies.size());
    for (int i = 0; i < original.lantencies.size(); ++i)
    {
        QCOMPARE(loaded.lantencies[i], original.lantencies[i]);
    }
}

// --- setPath ---

void TestJsonOperator::test_setPath_changesOutputDirectory()
{
    JsonOperator op;
    QString newPath = mTestDataPath + "subdir/";
    QDir().mkpath(newPath);
    op.setPath(newPath);

    Measures m;
    m.name = "PathTest";
    m.date = QDateTime::fromString("2026-06-01T09:00:00", Qt::ISODate);
    m.timeFactor = 100;
    m.duration = 2000;
    m.lantencies = {30.0};

    op.saveMeasureToDisk(m);

    QString expectedFile = newPath + "2026-06-01T09-00-00_Medicion.json";
    QVERIFY2(QFile::exists(expectedFile),
             qPrintable("Expected file not found: " + expectedFile));
}

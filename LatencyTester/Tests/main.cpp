#include <QApplication>
#include <QtTest>
#include "Core/tst_datamodel.h"
#include "Core/tst_appsettings.h"
#include "Core/tst_jsonoperator.h"
#include "Core/tst_sensoroperator.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    int status = 0;

    // --- Core tests ---
    {
        TestDataModel tst;
        status |= QTest::qExec(&tst, argc, argv);
    }
    {
        TestAppSettings tst;
        status |= QTest::qExec(&tst, argc, argv);
    }
    {
        TestJsonOperator tst;
        status |= QTest::qExec(&tst, argc, argv);
    }
    {
        TestSensorOperator tst;
        status |= QTest::qExec(&tst, argc, argv);
    }

    // Future test classes will be added here as they are implemented.

    return status;
}

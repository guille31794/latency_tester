#include "tst_datamodel.h"
#include "dataModel.hpp"
#include <QtTest>

// --- GeneralConfigSettings ---

void TestDataModel::test_generalConfig_defaultLanguageIsSpanish()
{
    GeneralConfigSettings config;
    QCOMPARE(config.language, Languages::SPANISH);
}

void TestDataModel::test_generalConfig_defaultFontSizeIs9()
{
    GeneralConfigSettings config;
    QCOMPARE(config.fontSize, quint8(9));
}

void TestDataModel::test_generalConfig_defaultDaltonicModeDisabled()
{
    GeneralConfigSettings config;
    QCOMPARE(config.daltonicMode, false);
}

void TestDataModel::test_generalConfig_defaultDarkModeDisabled()
{
    GeneralConfigSettings config;
    QCOMPARE(config.darkMode, false);
}

// --- Languages enum ---

void TestDataModel::test_languages_spanishIsZero()
{
    QCOMPARE(static_cast<int>(Languages::SPANISH), 0);
}

void TestDataModel::test_languages_englishIsOne()
{
    QCOMPARE(static_cast<int>(Languages::ENGLISH), 1);
}

void TestDataModel::test_languages_polishIsTwo()
{
    QCOMPARE(static_cast<int>(Languages::POLISH), 2);
}

// --- Colors struct ---

void TestDataModel::test_colors_defaultEmpty()
{
    Colors colors;
    QVERIFY(colors.backButton.isEmpty());
    QVERIFY(colors.widgets.isEmpty());
}

void TestDataModel::test_colors_assignValues()
{
    Colors colors;
    colors.backButton = "background-color: red;";
    colors.widgets = "background-color: blue;";
    QCOMPARE(colors.backButton, QString("background-color: red;"));
    QCOMPARE(colors.widgets, QString("background-color: blue;"));
}

// --- Measures struct ---

void TestDataModel::test_measures_defaultNameEmpty()
{
    Measures m;
    QVERIFY(m.name.isEmpty());
}

void TestDataModel::test_measures_defaultTimeFactor100()
{
    Measures m;
    QCOMPARE(m.timeFactor, 100);
}

void TestDataModel::test_measures_defaultDuration10000ms()
{
    Measures m;
    QCOMPARE(m.duration, 10000);
}

void TestDataModel::test_measures_defaultLatenciesEmpty()
{
    Measures m;
    QVERIFY(m.lantencies.isEmpty());
}

void TestDataModel::test_measures_defaultMeanLatencyZero()
{
    Measures m;
    QCOMPARE(m.meanLatency, 0.0);
}

void TestDataModel::test_measures_defaultDateInvalid()
{
    Measures m;
    QVERIFY(!m.date.isValid());
}

void TestDataModel::test_measures_latenciesAppend()
{
    Measures m;
    m.lantencies.append(150.5);
    m.lantencies.append(200.3);
    m.lantencies.append(-1.0);
    QCOMPARE(m.lantencies.size(), 3);
    QCOMPARE(m.lantencies[0], 150.5);
    QCOMPARE(m.lantencies[2], -1.0);
}

void TestDataModel::test_measures_zeroTimeFactor()
{
    Measures m;
    m.timeFactor = 0;
    QCOMPARE(m.timeFactor, 0);
}

void TestDataModel::test_measures_zeroDuration()
{
    Measures m;
    m.duration = 0;
    QCOMPARE(m.duration, 0);
}

void TestDataModel::test_measures_negativeDuration()
{
    Measures m;
    m.duration = -500;
    QCOMPARE(m.duration, -500);
}

void TestDataModel::test_measures_dateAssignment()
{
    Measures m;
    QDateTime now = QDateTime::currentDateTime();
    m.date = now;
    QVERIFY(m.date.isValid());
    QCOMPARE(m.date, now);
}

#ifndef TST_DATAMODEL_H
#define TST_DATAMODEL_H

#include <QObject>

class TestDataModel : public QObject
{
    Q_OBJECT

private slots:
    // GeneralConfigSettings
    void test_generalConfig_defaultLanguageIsSpanish();
    void test_generalConfig_defaultFontSizeIs9();
    void test_generalConfig_defaultDaltonicModeDisabled();
    void test_generalConfig_defaultDarkModeDisabled();

    // Languages enum
    void test_languages_spanishIsZero();
    void test_languages_englishIsOne();
    void test_languages_polishIsTwo();

    // Colors struct
    void test_colors_defaultEmpty();
    void test_colors_assignValues();

    // Measures struct
    void test_measures_defaultNameEmpty();
    void test_measures_defaultTimeFactor100();
    void test_measures_defaultDuration10000ms();
    void test_measures_defaultLatenciesEmpty();
    void test_measures_defaultMeanLatencyZero();
    void test_measures_defaultDateInvalid();
    void test_measures_latenciesAppend();
    void test_measures_zeroTimeFactor();
    void test_measures_zeroDuration();
    void test_measures_negativeDuration();
    void test_measures_dateAssignment();
};

#endif // TST_DATAMODEL_H

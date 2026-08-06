#include "tst_registryscreen.h"
#include "registryscreen.h"
#include <QtTest>
#include <QSignalSpy>
#include <QPushButton>
#include <QTreeView>

// --- Instantiation ---

void TestRegistryScreen::test_canInstantiate()
{
    RegistryScreen screen;
    QVERIFY(screen.findChild<QTreeView*>("registryTreeView"));
}

// --- Back button ---

void TestRegistryScreen::test_backButton_exists()
{
    RegistryScreen screen;
    QPushButton* btn = screen.findChild<QPushButton*>("backButton");
    QVERIFY(btn);
    QVERIFY(btn->isEnabled());
}

void TestRegistryScreen::test_backButton_emitsSignal()
{
    RegistryScreen screen;
    QSignalSpy spy(&screen, &RegistryScreen::backRequested);
    QPushButton* btn = screen.findChild<QPushButton*>("backButton");
    QVERIFY(btn);
    QTest::mouseClick(btn, Qt::LeftButton);
    QCOMPARE(spy.count(), 1);
}

// --- Action buttons ---

void TestRegistryScreen::test_checkButton_exists()
{
    RegistryScreen screen;
    QPushButton* btn = screen.findChild<QPushButton*>("checkRegistryEntryButton");
    QVERIFY(btn);
    QVERIFY(btn->isEnabled());
    QVERIFY(!btn->text().isEmpty());
}

void TestRegistryScreen::test_deleteButton_exists()
{
    RegistryScreen screen;
    QPushButton* btn = screen.findChild<QPushButton*>("deleteRegistryEntryButton");
    QVERIFY(btn);
    QVERIFY(btn->isEnabled());
    QVERIFY(!btn->text().isEmpty());
}

void TestRegistryScreen::test_renameButton_exists()
{
    RegistryScreen screen;
    QPushButton* btn = screen.findChild<QPushButton*>("renameRegistryEntryButton");
    QVERIFY(btn);
    QVERIFY(btn->isEnabled());
    QVERIFY(!btn->text().isEmpty());
}

// --- TreeView ---

void TestRegistryScreen::test_treeView_exists()
{
    RegistryScreen screen;
    QTreeView* tree = screen.findChild<QTreeView*>("registryTreeView");
    QVERIFY(tree);
    // Should have a model assigned (QFileSystemModel)
    QVERIFY(tree->model());
}

// --- Signals ---

void TestRegistryScreen::test_measureSelected_signalExists()
{
    RegistryScreen screen;
    // Verify signal is connectable (compile-time check effectively, but also runtime)
    QSignalSpy spy(&screen, &RegistryScreen::measureSelected);
    QVERIFY(spy.isValid());
}

void TestRegistryScreen::test_backRequested_signalExists()
{
    RegistryScreen screen;
    QSignalSpy spy(&screen, &RegistryScreen::backRequested);
    QVERIFY(spy.isValid());
}

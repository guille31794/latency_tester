#ifndef TST_REGISTRYSCREEN_H
#define TST_REGISTRYSCREEN_H

#include <QObject>
#include <QTemporaryDir>

class TestRegistryScreen : public QObject
{
    Q_OBJECT

private slots:
    // Instantiation
    void test_canInstantiate();

    // Back button
    void test_backButton_exists();
    void test_backButton_emitsSignal();

    // Action buttons exist
    void test_checkButton_exists();
    void test_deleteButton_exists();
    void test_renameButton_exists();

    // TreeView exists
    void test_treeView_exists();

    // Signals
    void test_measureSelected_signalExists();
    void test_backRequested_signalExists();
};

#endif // TST_REGISTRYSCREEN_H

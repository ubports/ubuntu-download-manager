#include "test_download.h"

TestDownload::TestDownload(QObject *parent)
    : LocalTreeTestCase("TestDownload", parent) {
}

void
TestDownload::init() {
    LocalTreeTestCase::init();
    _man = Manager::createSessionManager(daemonPath(), this);

    QVariantMap metadata;
    QMap<QString, QString> headers;
    QUrl notPresentFile = serverUrl();
    DownloadStruct downStruct(notPresentFile.toString(),
        metadata, headers);

    _down = _man->createDownload(downStruct);
}

void
TestDownload::cleanup(){
    LocalTreeTestCase::cleanup();
    delete _down;
    delete _man;
}

void
TestDownload::testAllowMobileDownload_data() {
    QTest::addColumn<bool>("enabled");

    QTest::newRow("Enabled") << true;
    QTest::newRow("Disabled") << false;
}

void
TestDownload::testAllowMobileDownload() {
    QFAIL("Not implemented.");
}

void
TestDownload::testAllowMobileDownloadError() {
    returnDBusErrors(true);
    _down->allowMobileDownload(false);
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
}

void
TestDownload::testSetThrottle_data() {
    QTest::addColumn<uint>("speed");

    QTest::newRow("First row") << 200u;
    QTest::newRow("Second row") << 1212u;
    QTest::newRow("Third row") << 998u;
    QTest::newRow("Last row") << 60u;
}

void
TestDownload::testSetThrottle() {
    QFAIL("Not implemented.");
}

void
TestDownload::testSetThrottleError() {
    returnDBusErrors(true);
    _down->setThrottle(0);
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
}

void
TestDownload::testMetadata() {
    QFAIL("Not implemented.");
}

void
TestDownload::testMetadataError() {
    returnDBusErrors(true);
    _down->metadata();
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
}

void
TestDownload::testProgress() {
    QFAIL("Not implemented.");
}

void
TestDownload::testProgressError() {
    returnDBusErrors(true);
    _down->progress();
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
}

void
TestDownload::testTotalSize() {
    QFAIL("Not implemented.");
}

void
TestDownload::testTotalSizeError() {
    returnDBusErrors(true);
    _down->totalSize();
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
}

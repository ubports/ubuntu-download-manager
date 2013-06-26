#include <QSignalSpy>
#include "fake_qnetwork_access_manager.h"
#include "test_app_download.h"

TestAppDownload::TestAppDownload(QObject* parent) :
    QObject(parent)
{
}

void TestAppDownload::init()
{
    _appId = "app-unique-id";
    _appName = "my super app";
    _path = "random path to dbus";
    _url = QUrl("http://ubuntu.com");
    _nam = new FakeQNetworkAccessManager();
}

void TestAppDownload::cleanup()
{
    if (_nam)
        delete _nam;
}

void TestAppDownload::testNoHashConstructor()
{
    QFAIL("Test not implemented");
}

void TestAppDownload::testHashConstructor()
{
    QFAIL("Test not implemented");
}

void TestAppDownload::testPath_data()
{
    // create a number of rows with a diff path to ensure that
    // the accesor does return the correct one
    QTest::addColumn<QString>("path");
    QTest::newRow("First row") << "/first/random/path";
    QTest::newRow("Second row") << "/second/random/path";
    QTest::newRow("Third row") << "/third/random/path";
    QTest::newRow("Last row") << "/last/random/path";
}

void TestAppDownload::testPath()
{
    // create an app download and assert that the returned data is correct
    QFETCH(QString, path);
    AppDownload* download = new AppDownload(_appId, _appName, path, _url, _nam);
    QCOMPARE(download->path(), path);
}

void TestAppDownload::testUrl_data()
{
    // create a number of rows with a diff url to ensure that
    // the accesor does return the correct one
    QTest::addColumn<QUrl>("url");
    QTest::newRow("First row") << QUrl("http://ubuntu.com");
    QTest::newRow("Second row") << QUrl("http://one.ubuntu.com");
    QTest::newRow("Third row") << QUrl("http://ubuntu.com/phone");
    QTest::newRow("Last row") << QUrl("http://ubuntu.com/tablet");
}

void TestAppDownload::testUrl()
{
    // create an app download and assert that the returned data is correct
    QFETCH(QUrl, url);
    AppDownload* download = new AppDownload(_appId, _appName, _path, url, _nam);
    QCOMPARE(download->url(), url);
}

void TestAppDownload::testApplicationId_data()
{
    // create a number of rows with a diff app id to ensure that
    // the accessor does return the correct one
    QTest::addColumn<QString>("appId");
    QTest::newRow("First row") << "my-first-app-id";
    QTest::newRow("Second row") << "my-second-app-id";
    QTest::newRow("Third row") << "my-third-app-id";
    QTest::newRow("Last row") << "my-last-app-id";
}

void TestAppDownload::testApplicationId()
{
    QFETCH(QString, appId);
    AppDownload* download = new AppDownload(appId, _appName, _path, _url, _nam);
    QCOMPARE(download->applicationId(), appId);
}

void TestAppDownload::testApplicationName_data()
{
    // create a number of rows with a diff app name to ensure that
    // the accessor does return the correct one
    QTest::addColumn<QString>("appName");
    QTest::newRow("First row") << "my-first-app-name";
    QTest::newRow("Second row") << "my-second-app-name";
    QTest::newRow("Third row") << "my-third-app-name";
    QTest::newRow("Last row") << "my-last-app-name";
}

void TestAppDownload::testApplicationName()
{
    QFETCH(QString, appName);
    AppDownload* download = new AppDownload(_appId, appName, _path, _url, _nam);
    QCOMPARE(download->applicationName(), appName);
}

void TestAppDownload::testMetadata()
{
    QFAIL("Test not implemented");
}

void TestAppDownload::testProgress()
{
    QFAIL("Test not implemented");
}

void TestAppDownload::testTotalSize()
{
    QFAIL("Test not implemented");
}

void TestAppDownload::testCancel()
{
    AppDownload* download = new AppDownload(_appId, _appName, _path, _url, _nam);
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->cancel();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), AppDownload::CANCELED);
}

void TestAppDownload::testPause()
{
    AppDownload* download = new AppDownload(_appId, _appName, _path, _url, _nam);
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->pause();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), AppDownload::PAUSED);
}

void TestAppDownload::testResume()
{
    AppDownload* download = new AppDownload(_appId, _appName, _path, _url, _nam);
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->resume();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), AppDownload::RESUMED);
}

void TestAppDownload::testStart()
{
    AppDownload* download = new AppDownload(_appId, _appName, _path, _url, _nam);
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->start();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), AppDownload::STARTED);
}

void TestAppDownload::testCancelDownload()
{
    QFAIL("Test not implemented");
}

void TestAppDownload::testPauseDownload()
{
    QFAIL("Test not implemented");
}

void TestAppDownload::testResumeDownload()
{
    QFAIL("Test not implemented");
}

void TestAppDownload::testStartDownload()
{
    QFAIL("Test not implemented");
}


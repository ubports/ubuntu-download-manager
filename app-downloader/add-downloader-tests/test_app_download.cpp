#include <QSignalSpy>
#include "fake_qnetwork_reply.h"
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
    _reqFactory = new FakeRequestFactory();
}

void TestAppDownload::cleanup()
{
    if (_reqFactory)
        delete _reqFactory;
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
    AppDownload* download = new AppDownload(_appId, _appName, path, _url, _reqFactory);
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
    AppDownload* download = new AppDownload(_appId, _appName, _path, url, _reqFactory);
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
    AppDownload* download = new AppDownload(appId, _appName, _path, _url, _reqFactory);
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
    AppDownload* download = new AppDownload(_appId, appName, _path, _url, _reqFactory);
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
    AppDownload* download = new AppDownload(_appId, _appName, _path, _url, _reqFactory);
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->cancel();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), AppDownload::CANCELED);
}

void TestAppDownload::testPause()
{
    AppDownload* download = new AppDownload(_appId, _appName, _path, _url, _reqFactory);
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->pause();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), AppDownload::PAUSED);
}

void TestAppDownload::testResume()
{
    AppDownload* download = new AppDownload(_appId, _appName, _path, _url, _reqFactory);
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->resume();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), AppDownload::RESUMED);
}

void TestAppDownload::testStart()
{
    AppDownload* download = new AppDownload(_appId, _appName, _path, _url, _reqFactory);
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->start();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), AppDownload::STARTED);
}

void TestAppDownload::testCancelDownload()
{
    // tell the fake nam to record so that we can access the reply

    _reqFactory->record();
    AppDownload* download = new AppDownload(_appId, _appName, _path, _url, _reqFactory);
    QSignalSpy spy(download , SIGNAL(canceled(bool)));

    download->start();  // change state
    download->startDownload();
    download->cancel(); // change state
    download->cancelDownload();  // method under test

    // assert that method was indeed called
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeQNetworkReply* reply = (FakeQNetworkReply*) calledMethods[0].params().outParams()[0];

    // assert that the reply was aborted and deleted via deleteLater
    calledMethods = reply->calledMethods();
    QCOMPARE(1, calledMethods.count());

    QCOMPARE(QString("abort"), calledMethods[0].methodName());

    // assert that the files do not exist in the system

}

void TestAppDownload::testCancelDownloadNotStarted()
{
    _reqFactory->record();
    AppDownload* download = new AppDownload(_appId, _appName, _path, _url, _reqFactory);
    QSignalSpy spy(download , SIGNAL(canceled(bool)));

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


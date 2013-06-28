#ifndef TEST_APP_DOWNLOAD_H
#define TEST_APP_DOWNLOAD_H

#include <QDir>
#include <QObject>
#include <app_download.h>
#include "fake_request_factory.h"
#include "test_runner.h"

class TestAppDownload: public QObject
{
    Q_OBJECT
public:
    explicit TestAppDownload(QObject *parent = 0);

private slots:

    void init();
    void cleanup();

    // constructors tests
    void testNoHashConstructor();
    void testHashConstructor();

    // data function to be used for the accessor tests
    void testNoHashConstructor_data();
    void testHashConstructor_data();
    void testPath_data();
    void testUrl_data();
    void testApplicationId_data();
    void testApplicationName_data();
    void testProgress_data();
    void testMetadata_data();

    // accessor methods
    void testPath();
    void testUrl();
    void testApplicationId();
    void testApplicationName();
    void testMetadata();
    void testProgress();
    void testTotalSize();
    void testTotalSizeNoProgress();

    // dbus method tests
    void testCancel();
    void testPause();
    void testResume();
    void testStart();

    // network related tests
    void testCancelDownload();
    void testCancelDownloadNotStarted();
    void testPauseDownload();
    void testPauseDownloadNotStarted();
    void testResumeDownload();
    void testStartDownload();
    void testStartDownloadAlreadyStarted();
    void testOnSuccessNoHash();
    void testOnSuccessHashError();
    void testOnSuccessHash();
    void testOnHttpError();

private:
    bool removeDir(const QString& dirName);

private:
    QDir _testDir;
    QString _appId;
    QString _appName;
    QString _path;
    QUrl _url;
    FakeRequestFactory* _reqFactory;

};

DECLARE_TEST(TestAppDownload)

#endif // TEST_APP_DOWNLOAD_H

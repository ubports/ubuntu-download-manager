#ifndef TEST_APP_DOWNLOAD_H
#define TEST_APP_DOWNLOAD_H

#include <QObject>

#include <QObject>
#include <app_download.h>
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
    void testPath_data();
    void testUrl_data();
    void testApplicationId_data();
    void testApplicationName_data();

    // accessor methods
    void testPath();
    void testUrl();
    void testApplicationId();
    void testApplicationName();
    void testMetadata();
    void testProgress();
    void testTotalSize();

    // dbus method tests
    void testCancel();
    void testPause();
    void testResume();
    void testStart();

    // network related tests
    void testCancelDownload();
    void testPauseDownload();
    void testResumeDownload();
    void testStartDownload();

private:
    QString _appId;
    QString _appName;
    QString _path;
    QUrl _url;
    QNetworkAccessManager* _nam;

};

DECLARE_TEST(TestAppDownload)

#endif // TEST_APP_DOWNLOAD_H

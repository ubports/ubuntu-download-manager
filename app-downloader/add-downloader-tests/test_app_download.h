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

    // constrictors tests
    void testNoHashConstructor();
    void testHashConstructor();

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

};

DECLARE_TEST(TestAppDownload)

#endif // TEST_APP_DOWNLOAD_H

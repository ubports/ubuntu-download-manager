#ifndef TEST_DOWNLOADER_H
#define TEST_DOWNLOADER_H

#include <QObject>
#include "test_runner.h"

class TestDownloader : public QObject
{
    Q_OBJECT
public:
    explicit TestDownloader(QObject *parent = 0);
    
public slots:

    void init();
    void cleanup();

    // test exposed via dbus
    void testCreateDownload();
    void testCreateDownloadWithHash();
    void testGetAllDownloads();

    // test q methods
    void testStartNoCurrent();
    void testStartCurrent();
    void testPauseCurrent();
    void testPauseNotCurrent();
    void testResumeCurrent();
    void testResumeNoCurrent();
};

DECLARE_TEST(TestDownloader)

#endif // TEST_DOWNLOADER_H

#ifndef TEST_DOWNLOAD_DAEMON_H
#define TEST_DOWNLOAD_DAEMON_H

#include <QObject>
#include "test_runner.h"

class TestDownloadDaemon : public QObject
{
    Q_OBJECT
public:
    explicit TestDownloadDaemon(QObject *parent = 0);
    
public slots:

    void init();
    void cleanup();
    void testStart();

};

DECLARE_TEST(TestDownloadDaemon)

#endif // TEST_DOWNLOAD_DAEMON_H

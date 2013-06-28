#ifndef TEST_NETWORK_REPLY_H
#define TEST_NETWORK_REPLY_H

#include <QObject>
#include <network_reply.h>
#include "test_runner.h"

class TestNetworkReply : public QObject
{
    Q_OBJECT
public:
    explicit TestNetworkReply(QObject *parent = 0);
    
private slots:

    void init();
    void cleanup();

    // data functions used in the tests
    void testDownloadProgressForwarded_data();

    void testDownloadProgressForwarded();
    void testErrorForwarded();
    void testFinishedForwarded();
    void testSslErrorsForwarded();
private:
    QNetworkReply* _qReply;
    NetworkReply* _reply;
};

DECLARE_TEST(TestNetworkReply)

#endif // TEST_NETWORK_REPLY_H

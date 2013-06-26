#ifndef FAKE_QNETWORK_REPLY_H
#define FAKE_QNETWORK_REPLY_H

#include <QNetworkReply>
#include "fake.h"


class FakeQNetworkReply : public QNetworkReply, public Fake
{

    Q_OBJECT
public:
    FakeQNetworkReply(QObject* parent=0);

    // fake methods used for the tests
    void abort();
    void deleteLater();

    // virtual methods that need to be implemented
    virtual qint64 bytesAvailable() const;
    virtual qint64 bytesToWrite() const;
    virtual bool canReadLine() const;
    virtual bool waitForReadyRead(int);
    virtual bool waitForBytesWritten(int);
    virtual qint64 readData(char* data, qint64 maxlen);
};

#endif // FAKE_QNETWORK_REPLY_H

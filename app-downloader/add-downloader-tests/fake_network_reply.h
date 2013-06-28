#ifndef FAKE_NETWORK_REPLY_H
#define FAKE_NETWORK_REPLY_H

#include <QObject>
#include <network_reply.h>
#include "fake.h"

class FakeNetworkReply : public NetworkReply, public Fake
{
    Q_OBJECT
public:
    explicit FakeNetworkReply(QObject *parent = 0);

    // access methods
    QByteArray data();
    void setData(QByteArray data);

    // fake methods

    QByteArray readAll() override;
    void abort() override;

private:
    QByteArray _data;
};

#endif // FAKE_NETWORK_REPLY_H

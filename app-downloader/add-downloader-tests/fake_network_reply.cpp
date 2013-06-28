#include "fake_qnetwork_reply.h"
#include "fake_network_reply.h"

FakeNetworkReply::FakeNetworkReply(QObject *parent) :
    NetworkReply(new FakeQNetworkReply(parent)),
    Fake()
{
}

QByteArray FakeNetworkReply::data()
{
    return _data;
}

void FakeNetworkReply::setData(QByteArray data)
{
    _data = data;
}

QByteArray FakeNetworkReply::readAll()
{
    if (_recording)
    {
        MethodData methodData;
        methodData.setMethodName("readAll");
        _called.append(methodData);
    }
    return _data;
}

void FakeNetworkReply::abort()
{
    if (_recording)
    {
        MethodData methodData;
        methodData.setMethodName("abort");
        _called.append(methodData);
    }
}

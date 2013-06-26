#include "fake_qnetwork_reply.h"

FakeQNetworkReply::FakeQNetworkReply(QObject* parent)
    : QNetworkReply(parent)
{
}

void FakeQNetworkReply::abort()
{
    if (_recording)
    {
        MethodData methodData;
        methodData.setMethodName("abort");
        _called.append(methodData);
    }
}

void FakeQNetworkReply::deleteLater()
{
    if (_recording)
    {
        MethodData methodData;
        methodData.setMethodName("deleteLater");
        _called.append(methodData);
    }

    QObject::deleteLater();
}

qint64 FakeQNetworkReply::bytesAvailable() const
{
    return 0;
}
qint64 FakeQNetworkReply::bytesToWrite() const
{
    return -1;
}

bool FakeQNetworkReply::canReadLine() const {
    qFatal("not implemented"); return false;
}

bool FakeQNetworkReply::waitForReadyRead(int) {
    qFatal("not implemented"); return false;
}

bool FakeQNetworkReply::waitForBytesWritten(int) {
    qFatal("not implemented"); return false;
}

qint64 FakeQNetworkReply::readData(char* data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)
    return 0;
}

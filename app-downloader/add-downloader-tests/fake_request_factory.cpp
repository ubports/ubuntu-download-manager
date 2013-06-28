#include <QDebug>
#include "fake_network_reply.h"
#include "fake_request_factory.h"

RequestWrapper::RequestWrapper(const QNetworkRequest& request, QObject* parent)
    : QObject(parent),
    _request(request)
{
}

QNetworkRequest RequestWrapper::request()
{
    return _request;
}

FakeRequestFactory::FakeRequestFactory(QObject *parent) :
    RequestFactory(parent),
    Fake()
{
}

NetworkReply* FakeRequestFactory::get(const QNetworkRequest& request)
{
    // return a FakeQNetworkReply
    FakeNetworkReply* reply = new FakeNetworkReply();

    if (_recording)
    {
        QList<QObject*> inParams;
        inParams.append(new RequestWrapper(request));

        QList<QObject*> outParams;
        outParams.append(reply);
        MethodParams params(inParams, outParams);

        MethodData methodData("get", params);
        _called.append(methodData);

        // if we are recording we do set the recording of the returned reply
        reply->record();
    }
    return reply;
}

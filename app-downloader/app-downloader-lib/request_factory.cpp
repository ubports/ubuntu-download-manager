#include <QNetworkAccessManager>
#include "request_factory.h"

/*
 * PRIVATE IMPLEMENTATION
 */

class RequestFactoryPrivate
{
    Q_DECLARE_PUBLIC(RequestFactory)

public:
    explicit RequestFactoryPrivate(RequestFactory* parent);

    NetworkReply* get(const QNetworkRequest& request);
private:
    QNetworkAccessManager* _nam;
    RequestFactory* q_ptr;

};

RequestFactoryPrivate::RequestFactoryPrivate(RequestFactory* parent) :
    q_ptr(parent)
{
    _nam = new QNetworkAccessManager();
}

NetworkReply* RequestFactoryPrivate::get(const QNetworkRequest& request)
{
    return new NetworkReply(_nam->get(request));
}

/*
 * PUBLIC IMPLEMENTATION
 */

RequestFactory::RequestFactory(QObject *parent) :
    QObject(parent),
    d_ptr(new RequestFactoryPrivate(this))
{
}

NetworkReply* RequestFactory::get(const QNetworkRequest& request)
{
    Q_D(RequestFactory);
    return d->get(request);
}

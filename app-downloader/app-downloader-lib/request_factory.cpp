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

    QNetworkReply* get(const QNetworkRequest& request);
private:
    QNetworkAccessManager* _nam;
    RequestFactory* q_ptr;

};

RequestFactoryPrivate::RequestFactoryPrivate(RequestFactory* parent) :
    q_ptr(parent)
{
    _nam = new QNetworkAccessManager();
}

QNetworkReply* RequestFactoryPrivate::get(const QNetworkRequest& request)
{
    return _nam->get(request);
}

/*
 * PUBLIC IMPLEMENTATION
 */

RequestFactory::RequestFactory(QObject *parent) :
    QObject(parent),
    d_ptr(new RequestFactoryPrivate(this))
{
}

QNetworkReply* RequestFactory::get(const QNetworkRequest& request)
{
    Q_D(RequestFactory);
    return d->get(request);
}

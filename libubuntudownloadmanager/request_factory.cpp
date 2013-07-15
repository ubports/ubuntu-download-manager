/*
 * Copyright 2013 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

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
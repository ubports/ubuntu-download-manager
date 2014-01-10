/*
 * Copyright 2013-2014 Canonical Ltd.
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

#include <QDebug>
#include "network_reply.h"
#include "request_factory.h"

RequestWrapper::RequestWrapper(const QNetworkRequest& request, QObject* parent)
    : QObject(parent),
    _request(request) {
}

QNetworkRequest
RequestWrapper::request() {
    return _request;
}

FakeRequestFactory::FakeRequestFactory(QObject *parent)
    : RequestFactory(parent),
      Fake() {
}

NetworkReply*
FakeRequestFactory::get(const QNetworkRequest& request) {
    // return a FakeQNetworkReply
    FakeNetworkReply* reply = new FakeNetworkReply(this);

    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new RequestWrapper(request, this));

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

QList<QSslCertificate>
FakeRequestFactory::acceptedCertificates() {
    if (_recording) {
        QList<QObject*> inParams;
        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("acceptedCertificates", params);
        _called.append(methodData);
    }
    return RequestFactory::acceptedCertificates();
}

void
FakeRequestFactory::setAcceptedCertificates(
                                const QList<QSslCertificate>& certs) {
    if (_recording) {
        QList<QObject*> inParams;
        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("setAcceptedCertificates", params);
        _called.append(methodData);
    }
    RequestFactory::setAcceptedCertificates(certs);
}

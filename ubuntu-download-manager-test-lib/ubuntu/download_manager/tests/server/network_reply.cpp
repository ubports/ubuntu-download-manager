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

#include "qnetwork_reply.h"
#include "network_reply.h"


SslErrorsListWrapper::SslErrorsListWrapper(QList<QSslError> value,
                                           QObject* parent)
    : QObject(parent) {
    _value = value;
}

QList<QSslError>
SslErrorsListWrapper::value() {
    return _value;
}

void
SslErrorsListWrapper::setValue(QList<QSslError> value) {
    _value = value;
}

FakeNetworkReply::FakeNetworkReply(QObject *parent)
    : NetworkReply(new FakeQNetworkReply(), parent),
      Fake() {
}

QByteArray
FakeNetworkReply::data() {
    return _data;
}

void
FakeNetworkReply::setData(QByteArray data) {
    _data = data;
}

QByteArray
FakeNetworkReply::readAll() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("readAll");
        _called.append(methodData);
    }
    return _data;
}

void
FakeNetworkReply::abort() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("abort");
        _called.append(methodData);
    }
}

void
FakeNetworkReply::setReadBufferSize(uint size) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new UintWrapper(size, this));

        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("setReadBufferSize", params);
        _called.append(methodData);
    }
}

void
FakeNetworkReply::emitFinished() {
    emit finished();
}

void
FakeNetworkReply::setAcceptedCertificates(const QList<QSslCertificate>& certs) {
    if (_recording) {
        QList<QObject*> inParams;
        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("setAcceptedCertificates", params);
        _called.append(methodData);
    }
    NetworkReply::setAcceptedCertificates(certs);
}

bool
FakeNetworkReply::canIgnoreSslErrors(const QList<QSslError>& errors) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new SslErrorsListWrapper(errors, this));

        QList<QObject*> outParams;
        outParams.append(new BoolWrapper(_canIgnoreSsl, this));
        MethodParams params(inParams, outParams);
        MethodData methodData("canIgnoreSslErrors", params);
        _called.append(methodData);
    }
    return _canIgnoreSsl;
}

void
FakeNetworkReply::setCanIgnoreSslErrors(bool canIgnore) {
    _canIgnoreSsl = canIgnore;
}

void
FakeNetworkReply::emitSslErrors(const QList<QSslError>& errors) {
    emit sslErrors(errors);
}

void
FakeNetworkReply::emitHttpError(QNetworkReply::NetworkError code) {
    emit error(code);
}

QVariant
FakeNetworkReply::attribute(QNetworkRequest::Attribute code) const {
    if (_attrs.contains(code)) {
        return _attrs[code];
    } else {
        QVariant var(QVariant::Invalid);
        return var;
    }
}

void
FakeNetworkReply::setAttribute(QNetworkRequest::Attribute code,
                               int value) {
    _attrs[code] = value;
}

void
FakeNetworkReply::setAttribute(QNetworkRequest::Attribute code,
                               QString message) {
    _attrs[code] = message;
}

void
FakeNetworkReply::setAttribute(QNetworkRequest::Attribute code,
                               QUrl url) {
    _attrs[code] = url;
}

void
FakeNetworkReply::clearAttribute(QNetworkRequest::Attribute code) {
    if (_attrs.contains(code)) {
        _attrs.remove(code);
    }
}

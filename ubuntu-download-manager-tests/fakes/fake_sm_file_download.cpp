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

#include "fake_sm_file_download.h"

NetworkErrorWrapper::NetworkErrorWrapper(QNetworkReply::NetworkError value,
                                         QObject* parent)
    : QObject(parent) {
    _value = value;
}

QNetworkReply::NetworkError
NetworkErrorWrapper::value() {
    return _value;
}

void
NetworkErrorWrapper::setValue(QNetworkReply::NetworkError value) {
    _value = value;
}

SslErrorsWrapper::SslErrorsWrapper(QList<QSslError> value,
                                   QObject* parent)
    : QObject(parent){
    _value = value;
}

QList<QSslError>
SslErrorsWrapper::value() {
    return _value;
}

void
SslErrorsWrapper::setValue(QList<QSslError> value) {
    _value = value;
}

FakeSMFileDownload::FakeSMFileDownload(QObject *parent)
    : SMFileDownload(parent) {
}

void
FakeSMFileDownload::emitError(QString error) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new StringWrapper(error, this));

        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("emitError", params);
        _called.append(methodData);
    }
}

void
FakeSMFileDownload::emitNetworkError(QNetworkReply::NetworkError code) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new NetworkErrorWrapper(code, this));

        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("emitNetworkError", params);
        _called.append(methodData);
    }
}

void
FakeSMFileDownload::emitSslError(const QList<QSslError>& errors) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new SslErrorsWrapper(errors, this));

        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("emitSslError", params);
        _called.append(methodData);
    }
}

Download::State
FakeSMFileDownload::state() {
    return _state;
}

void
FakeSMFileDownload::setState(Download::State state) {
    _state = state;
}

void
FakeSMFileDownload::requestDownload() {
    if (_recording) {
        QList<QObject*> inParams;

        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("requestDownload", params);
        _called.append(methodData);
    }
}

void
FakeSMFileDownload::pauseRequestDownload() {
    if (_recording) {
        QList<QObject*> inParams;

        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("pauseRequestDownload", params);
        _called.append(methodData);
    }
}

void
FakeSMFileDownload::cancelRequestDownload() {
    if (_recording) {
        QList<QObject*> inParams;

        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("cancelRequestDownload", params);
        _called.append(methodData);
    }
}

void
FakeSMFileDownload::raiseHeadRequestCompleted() {
    emit headRequestCompleted();
}

void
FakeSMFileDownload::raiseNetworkError(QNetworkReply::NetworkError code) {
    emit error(code);
}

void
FakeSMFileDownload::raiseSslError(const QList<QSslError>& errors) {
    emit sslErrors(errors);
}

void
FakeSMFileDownload::raiseConnectionEnabled() {
    emit connectionEnabled();
}

void
FakeSMFileDownload::raiseConnectionDisabled() {
    emit connectionDisabled();
}

void
FakeSMFileDownload::raiseDownloadingStarted() {
    emit downloadingStarted();
}

void
FakeSMFileDownload::raisePaused() {
    emit paused();
}

void
FakeSMFileDownload::raiseCompleted() {
    emit completed();
}

void
FakeSMFileDownload::raiseHashingStarted() {
    emit hashingStarted();
}

void
FakeSMFileDownload::raiseHashingError() {
    emit hashingError();
}

void
FakeSMFileDownload::raisePostProcessingStarted() {
    emit postProcessingStarted();
}

void
FakeSMFileDownload::raisePostProcessingError() {
    emit postProcessingError();
}

void
FakeSMFileDownload::raiseCanceled() {
    emit canceled();
}

void
FakeSMFileDownload::raiseFinished() {
    emit finished();
}

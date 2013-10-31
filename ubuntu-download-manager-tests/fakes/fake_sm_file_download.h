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

#ifndef FAKE_SM_FILED_OWNLOAD_H
#define FAKE_SM_FILED_OWNLOAD_H

#include <QObject>
#include <QSslError>
#include <downloads/sm_file_download.h>
#include "fakes/fake.h"

using namespace Ubuntu::DownloadManager;

class NetworkErrorWrapper : public QObject {
    Q_OBJECT
 public:
    NetworkErrorWrapper(QNetworkReply::NetworkError value, QObject* parent = 0);

    QNetworkReply::NetworkError value();
    void setValue(QNetworkReply::NetworkError value);

 private:
    QNetworkReply::NetworkError _value;
};

class SslErrorsWrapper : public QObject {
    Q_OBJECT
 public:
    SslErrorsWrapper(QList<QSslError> value, QObject* parent = 0);

    QList<QSslError> value();
    void setValue(QList<QSslError> value);

 private:
    QList<QSslError> _value;
};

class FakeSMFileDownload : public SMFileDownload, public Fake{
    Q_OBJECT
 public:
    explicit FakeSMFileDownload(QObject *parent = 0);
    
    void emitError(QString error) override;
    void emitNetworkError(QNetworkReply::NetworkError code) override;
    void emitSslError(const QList<QSslError>& errors) override;

    void raiseNetworkError(QNetworkReply::NetworkError code);
    void raiseSslError(const QList<QSslError>& errors);
};

#endif  // FAKE_SM_FILED_OWNLOAD_H

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

#ifndef FAKE_QNETWORK_REPLY_H
#define FAKE_QNETWORK_REPLY_H

#include <QNetworkReply>
#include "ubuntu/download_manager/tests/fake.h"


class FakeQNetworkReply : public QNetworkReply, public Fake {
    Q_OBJECT

 public:
    explicit FakeQNetworkReply(QObject* parent = 0);

    // fake methods used for the tests
    void abort();
    void deleteLater();

    // virtual methods that need to be implemented
    virtual qint64 bytesAvailable() const;
    virtual qint64 bytesToWrite() const;
    virtual bool canReadLine() const;
    virtual bool waitForReadyRead(int data);
    virtual bool waitForBytesWritten(int data);
    virtual qint64 readData(char* data, qint64 maxlen);
};

#endif  // FAKE_QNETWORK_REPLY_H

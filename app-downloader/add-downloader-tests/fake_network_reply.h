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

#ifndef FAKE_NETWORK_REPLY_H
#define FAKE_NETWORK_REPLY_H

#include <QObject>
#include <network_reply.h>
#include "fake.h"

class FakeNetworkReply : public NetworkReply, public Fake
{
    Q_OBJECT
public:
    explicit FakeNetworkReply(QObject *parent = 0);

    // access methods
    QByteArray data();
    void setData(QByteArray data);

    // fake methods

    QByteArray readAll() override;
    void abort() override;

private:
    QByteArray _data;
};

#endif // FAKE_NETWORK_REPLY_H

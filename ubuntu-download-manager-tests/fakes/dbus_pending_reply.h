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

#ifndef DBUS_PENDING_CALL_H
#define DBUS_PENDING_CALL_H

#include <ubuntu/download_manager/dbus/pending_reply.h>
#include <QObject>
#include "fake.h"

using namespace Ubuntu::DownloadManager::DBus;

class FakeDBusPendingReply : public PendingReply, public Fake {
    Q_OBJECT

public:
    FakeDBusPendingReply(QDBusPendingCallWatcher* watch, QObject *parent = 0);
    
    bool isError() override;
    void setIsError(bool isError);

    QDBusError error() const override;
    void setError(QDBusError err);

private:
    bool _isError;
    QDBusError _err;
    
};

#endif // DBUS_PENDING_CALL_H

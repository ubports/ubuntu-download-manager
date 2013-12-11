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

#include "dbus_pending_reply.h"

FakeDBusPendingReply::FakeDBusPendingReply(QDBusPendingCallWatcher* watch, QObject* parent)
    : PendingReply(watch, parent),
      Fake() {
}

bool
FakeDBusPendingReply::isError() {
    return _isError;
}

void
FakeDBusPendingReply::setIsError(bool isError) {
    _isError = isError;
}


QDBusError
FakeDBusPendingReply::error() const {
    return _err;
}

void
FakeDBusPendingReply::setError(QDBusError err) {
    _err = err;
}

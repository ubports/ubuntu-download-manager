/*
 * Copyright 2014 Canonical Ltd.
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

#ifndef DOWNLOADER_LIB_PENDING_REPLY_H
#define DOWNLOADER_LIB_PENDING_REPLY_H

#include <QDBusPendingReply>

namespace Ubuntu {

namespace Transfers {

namespace System {

template <class T>
class PendingReply {
 public:
    PendingReply() {
    }

    PendingReply(const QDBusPendingReply<T>& reply)
        : _reply(reply) {
    }

    PendingReply(const PendingReply<T>& other)
        : _reply(other._reply) {
    }

    // added to help mocking
    virtual ~PendingReply() {
    }

    virtual T value () const {
        return _reply.value();
    }

    virtual bool isError () const {
        return _reply.isError();
    }

    virtual QDBusError error() const {
        return _reply.error();
    }

    virtual void waitForFinished() {
        _reply.waitForFinished();
    }

 private:
    QDBusPendingReply<T> _reply;
};

}  // System

}  // Transfers

}  // Ubuntu

#endif  // DOWNLOADER_LIB_PENDING_REPLY_H

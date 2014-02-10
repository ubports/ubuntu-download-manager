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

#ifndef UBUNTU_DOWNLOADMANAGER_CLIENT_ERROR_H
#define UBUNTU_DOWNLOADMANAGER_CLIENT_ERROR_H

#include <QObject>
#include <QProcess>
#include "ubuntu-download-manager-client_global.h"

class QDBusError;

namespace Ubuntu {

namespace DownloadManager {

class HttpErrorStruct;
class NetworkErrorStruct;
class ProcessErrorStruct;
class ErrorPrivate;
class UBUNTUDOWNLOADMANAGERCLIENTSHARED_EXPORT Error : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(Error)

 public:
    enum Type {
        DBus,
        Http,
        Network,
        Process
    };

    virtual ~Error();
    Type type();

 protected:
    Error(Type type, QObject* parent = 0);

 private:
    // use pimpl pattern so that users do not have to be recompiled
    ErrorPrivate* d_ptr;

};


class DBusErrorPrivate;
class UBUNTUDOWNLOADMANAGERCLIENTSHARED_EXPORT DBusError : public Error {
    Q_OBJECT
    Q_DECLARE_PRIVATE(DBusError)

    friend class ManagerPrivate;
    friend class DownloadPrivate;
    friend class DownloadManagerPendingCallWatcher;
    friend class DownloadPendingCallWatcher;
    friend class GroupManagerPendingCallWatcher;

    virtual ~DBusError();
    QString message();
    QString name();

 protected:
    DBusError(QDBusError err, QObject* parent = 0);

 private:
    // use pimpl pattern so that users do not have to be recompiled
    DBusErrorPrivate* d_ptr;
};

class HttpErrorPrivate;
class UBUNTUDOWNLOADMANAGERCLIENTSHARED_EXPORT HttpError : public Error {
    Q_OBJECT
    Q_DECLARE_PRIVATE(HttpError)

    friend class DownloadPrivate;

 public:
    virtual ~HttpError();
    int code();
    QString phrase();

 protected:
    HttpError(HttpErrorStruct err, QObject* parent);

 private:
    // use pimpl pattern so that users do not have to be recompiled
    HttpErrorPrivate* d_ptr;
};

class NetworkErrorPrivate;
class UBUNTUDOWNLOADMANAGERCLIENTSHARED_EXPORT NetworkError : public Error {
    Q_OBJECT
    Q_DECLARE_PRIVATE(NetworkError)

    friend class DownloadPrivate;

 public:
    enum ErrorCode {
        ConnectionRefusedError,
        RemoteHostClosedError,
        HostNotFoundError,
        TimeoutError,
        OperationCanceledError,
        SslHandshakeFailedError,
        TemporaryNetworkFailureError,
        NetworkSessionFailedError,
        BackgroundRequestNotAllowedError,
        ProxyConnectionRefusedError,
        ProxyConnectionClosedError,
        ProxyNotFoundError,
        ProxyTimeoutError,
        ProxyAuthenticationRequiredError,
        ContentAccessDenied,
        ContentOperationNotPermittedError,
        ContentNotFoundError,
        AuthenticationRequiredError,
        ContentReSendError,
        ProtocolUnknownError,
        ProtocolInvalidOperationError,
        UnknownNetworkError,
        UnknownProxyError,
        UnknownContentError,
        ProtocolFailure
    };

    virtual ~NetworkError();
    ErrorCode code();
    QString phrase();

 protected:
    NetworkError(NetworkErrorStruct err, QObject* parent);

 private:
    // use pimpl pattern so that users do not have to be recompiled
    NetworkErrorPrivate* d_ptr;
};

class ProcessErrorPrivate;
class UBUNTUDOWNLOADMANAGERCLIENTSHARED_EXPORT ProcessError : public Error {
    Q_OBJECT
    Q_DECLARE_PRIVATE(ProcessError)

    friend class DownloadPrivate;

 public:
    virtual ~ProcessError();
    QProcess::ProcessError code();
    QString phrase();
    int exitCode();
    QString standardOut();
    QString standardError();

 protected:
    ProcessError(ProcessErrorStruct err, QObject* parent);

 private:
    // use pimpl pattern so that users do not have to be recompiled
    ProcessErrorPrivate* d_ptr;
};

}  // DownloadManager

}  // Ubuntu

#endif // ERROR_H

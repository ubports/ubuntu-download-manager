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
#include <ubuntu/download_manager/common.h>

class QDBusError;

namespace Ubuntu {

namespace DownloadManager {

class AuthErrorStruct;
class HttpErrorStruct;
class NetworkErrorStruct;
class ProcessErrorStruct;
class ErrorPrivate;
class DOWNLOAD_MANAGER_EXPORT Error : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(Error)

 public:
    enum Type {
        Auth,
        DBus,
        Http,
        Network,
        Process
    };

    virtual ~Error();
    Type type();
    virtual QString errorString();

 protected:
    Error(Type type, QObject* parent = 0);

 private:
    // use pimpl pattern so that users do not have to be recompiled
    ErrorPrivate* d_ptr;

};


class DBusErrorPrivate;
class DOWNLOAD_MANAGER_EXPORT DBusError : public Error {
    Q_OBJECT
    Q_DECLARE_PRIVATE(DBusError)

    friend class ManagerImpl;
    friend class DownloadImpl;
    friend class DownloadManagerPCW;
    friend class DownloadsListManagerPCW;
    friend class MetadataDownloadsListManagerPCW;
    friend class DownloadPCW;
    friend class GroupManagerPCW;

    virtual ~DBusError();
    QString message();
    QString name();
    QString errorString() override;

 protected:
    DBusError(QDBusError err, QObject* parent = 0);

 private:
    // use pimpl pattern so that users do not have to be recompiled
    DBusErrorPrivate* d_ptr;
};

class AuthErrorPrivate;
class DOWNLOAD_MANAGER_EXPORT AuthError : public Error {
    Q_OBJECT
    Q_DECLARE_PRIVATE(AuthError)

    friend class DownloadImpl;

 public:
    enum Type {
        Server,
        Proxy
    };

    virtual ~AuthError();
    Type type();
    QString phrase();
    QString errorString() override;

 protected:
    AuthError(AuthErrorStruct err, QObject* parent);

 private:
    // use pimpl pattern so that users do not have to be recompiled
    AuthErrorPrivate* d_ptr;
};

class HttpErrorPrivate;
class DOWNLOAD_MANAGER_EXPORT HttpError : public Error {
    Q_OBJECT
    Q_DECLARE_PRIVATE(HttpError)

    friend class DownloadImpl;

 public:
    virtual ~HttpError();
    int code();
    QString phrase();
    QString errorString() override;

 protected:
    HttpError(HttpErrorStruct err, QObject* parent);

 private:
    // use pimpl pattern so that users do not have to be recompiled
    HttpErrorPrivate* d_ptr;
};

class NetworkErrorPrivate;
class DOWNLOAD_MANAGER_EXPORT NetworkError : public Error {
    Q_OBJECT
    Q_DECLARE_PRIVATE(NetworkError)

    friend class DownloadImpl;

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
    QString errorString() override;

 protected:
    NetworkError(NetworkErrorStruct err, QObject* parent);

 private:
    // use pimpl pattern so that users do not have to be recompiled
    NetworkErrorPrivate* d_ptr;
};

class ProcessErrorPrivate;
class DOWNLOAD_MANAGER_EXPORT ProcessError : public Error {
    Q_OBJECT
    Q_DECLARE_PRIVATE(ProcessError)

    friend class DownloadImpl;

 public:
    virtual ~ProcessError();
    QProcess::ProcessError code();
    QString phrase();
    int exitCode();
    QString standardOut();
    QString standardError();
    QString errorString() override;

 protected:
    ProcessError(ProcessErrorStruct err, QObject* parent);

 private:
    // use pimpl pattern so that users do not have to be recompiled
    ProcessErrorPrivate* d_ptr;
};

}  // DownloadManager

}  // Ubuntu

#endif // ERROR_H

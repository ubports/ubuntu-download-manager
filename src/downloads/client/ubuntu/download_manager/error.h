/*
 * Copyright 2013-2015 Canonical Ltd.
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

#pragma once

#include <QObject>
#include <QProcess>

class QDBusError;

namespace Ubuntu {

namespace Transfers {

namespace Errors {
    class AuthErrorStruct;
    class HttpErrorStruct;
    class NetworkErrorStruct;
    class ProcessErrorStruct;
    class HashErrorStruct;
}

}

namespace DownloadManager {

class ErrorPrivate;

/*!
    \class Error
    \brief The Error class is the base class that represents an
           error in the download manager API.
    \since 0.3

    The Error class represents an error in the download manager and
    can be of a number of types. The API is designed so that the client
    only needs to listen to a single error signal and that can later
    downcast the error class to get more detailed information.
*/
class Error : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(Error)

 public:

    /*!
        Enumerator that represents the error type.
    */
    enum Type {
        Auth,    /*! The error was due to a proxy or server authentication issue.*/
        DBus,    /*! The error was due to some problem when communicating with the dbus API.*/
        Http,    /*! The error was due to a http error, for example a 404.*/
        Network, /*! The error was due to network problems such as a missing network interface.*/
        Process, /*! The error was due to a problem when executing the post download command.*/
        Hash     /*! The error was due to a problem when verifying the checksum of the download.*/
    };

    /*!
        Disposes of this error and frees any resources associated with it.
    */
    virtual ~Error();

    /*!
        \fn void Error::Type()

        Returns the type of the error that was encountered.
     */
    Type type();

    /*!
        \fn QString errorString()

        Returns a string representation of the error.
    */
    virtual QString errorString();

 protected:
    /*!
        Creates a new error of the given type and with the given parent.
        \internal
    */
    Error(Type type, QObject* parent = 0);

 private:
    /*!
        Use pimpl pattern so that users do not have to be recompiled
        \internal
    */
    ErrorPrivate* d_ptr;

};


class DBusErrorPrivate;

/*!
    \class DbusError
    \brief The DbusError represents an error that occurred during the
           interaction with the DBus API.
    \since 0.3
*/
class DBusError : public Error {
    Q_OBJECT
    Q_DECLARE_PRIVATE(DBusError)

    friend class ManagerImpl;
    friend class DownloadImpl;
    friend class DownloadManagerPCW;
    friend class DownloadsListManagerPCW;
    friend class MetadataDownloadsListManagerPCW;
    friend class DownloadPCW;
    friend class GroupManagerPCW;

    /*!
        Disposes of this error and frees any resources associated with it.
    */
    virtual ~DBusError();

    /*!
        \fn QString message()

        Returns a DBus error message that is readable by a human.
    */
    QString message();

    /*!
        \fn QString name();

        Returns the name of the DBus error.
    */
    QString name();

    /*!
        \fn QString errorString() override

        Returns a string representation of the error.
    */
    QString errorString() override;

 protected:

    /*!
        Creates a new error from the provided QDbusError with a
        given parent.
        \internal
    */
    DBusError(QDBusError err, QObject* parent = 0);

 private:
    /*!
        Use pimpl pattern so that users do not have to be recompiled
        \internal
    */
    DBusErrorPrivate* d_ptr;
};

class AuthErrorPrivate;

/*!
    \class AuthError
    \brief The AuthError represents an authentication error that occurred
           during the request of the download.
    \since 0.3

    The AuthError can occur due to two different reasons, either the
    download required a server side authentication or the download was
    performed via a proxy that requires authentication and was not
    provided.
*/
class AuthError : public Error {
    Q_OBJECT
    Q_DECLARE_PRIVATE(AuthError)

    friend class DownloadImpl;

 public:

    /*!
        Enumerator that represents the type of authentication error.
    */
    enum Type {
        Server, /*! The server needed authentication and it was not provided.*/
        Proxy   /*! The proxy needed authentication and it was not provided. */
    };

    /*!
        Disposes of this error and frees any resources associated with it.
    */
    virtual ~AuthError();

    /*!
        /fn Type type()

        Returns the type of authentication error.
    */
    Type type();

    /*!
        /fn QString phrase()

        Returns a human readable explanation for the authentication
        error.
    */
    QString phrase();

    /*!
        /fn QString errorString() override

        Returns a string representation of the error.
    */
    QString errorString() override;

 protected:
    /*!
        Creates a new error from the provided authentication error struct.
        \internal
    */
    AuthError(Transfers::Errors::AuthErrorStruct err, QObject* parent);

 private:
    /*!
        Use pimpl pattern so that users do not have to be recompiled
        \internal
    */
    AuthErrorPrivate* d_ptr;
};

class HttpErrorPrivate;

/*!
    \class HttpError 
    \brief The HttpError represents an error that occurred during the
           download request.
    \since 0.3

    The HttpError represents an error that occurred during the download
    request and that is related to one of the known http 1.1 error codes.
*/
class HttpError : public Error {
    Q_OBJECT
    Q_DECLARE_PRIVATE(HttpError)

    friend class DownloadImpl;

 public:

    /*!
        Disposes of this error and frees any resources associated with it.
    */
    virtual ~HttpError();

    /*!
        /fn int code()

        Returns the http error code, for example 404.
    */
    int code();

    /*!
        /fn QString phrase()

        Returns a human readable reason for the http error.
    */
    QString phrase();

    /*!
        /fn QString errorString() override

        Returns a string representation of the error.
    */
    QString errorString() override;

 protected:
    /*!
        Creates a new error from the provided http error struct.
        \internal
    */
    HttpError(Transfers::Errors::HttpErrorStruct err, QObject* parent);

 private:
    /*!
        Use pimpl pattern so that users do not have to be recompiled
        \internal
    */
    HttpErrorPrivate* d_ptr;
};

class NetworkErrorPrivate;

/*!
    \class NetworkError
    \brief The NetworkError represents an error that occurred during the
           download request.
    \since 0.3
*/
class NetworkError : public Error {
    Q_OBJECT
    Q_DECLARE_PRIVATE(NetworkError)

    friend class DownloadImpl;

 public:

    /*!
      Enumerator that indicates all possible error conditions found during
      the processing of the request.
    */
    enum ErrorCode {
        ConnectionRefusedError,            /*! The remote server refused the connection (the server is not accepting requests). */
        RemoteHostClosedError,             /*! The remote server closed the connection prematurely, before the entire reply was received and processed. */
        HostNotFoundError,                 /*! The remote host name was not found (invalid hostname). */
        TimeoutError,                      /*! The connection to the remote server timed out. */
        OperationCanceledError,            /*! The operation was canceled. */
        SslHandshakeFailedError,           /*! The  SSL/TLS handshake failed and the encrypted channel could not be established. */
        TemporaryNetworkFailureError,      /*! The connection was broken due to disconnection from the networkd. */
        NetworkSessionFailedError,         /*! The connection was broken due to disconnection from the network or failure to start the network. */
        BackgroundRequestNotAllowedError,  /*! The background request is not currently allowed due to platform policy. */
        ProxyConnectionRefusedError,       /*! The connection to the proxy server was refused (the proxy server is not accepting requests. )*/
        ProxyConnectionClosedError,        /*! The proxy server closed the connection prematurely, before the entire reply was received and processed. */
        ProxyNotFoundError,                /*! The proxy host name was not found (invalid proxy hostname). */
        ProxyTimeoutError,                 /*! The connection to the proxy timed out or the proxy did not reply in time to the request sent. */
        ProxyAuthenticationRequiredError,  /*! The proxy requires authentication in order to honour the request but did not accept any credentials offered.*/
        ContentAccessDenied,               /*! The access to the remote content was denied. */
        ContentOperationNotPermittedError, /*! The operation requested on the remote content is not permitted. */
        ContentNotFoundError,              /*! The remote content was not found at the server. */
        AuthenticationRequiredError,       /*! The remote server requires authentication to serve the content but the credentials provided were not accepted. */
        ContentReSendError,                /*! The request needed to be sent again, but this failed. */
        ProtocolUnknownError,              /*! The Network Access API cannot honor the request because the protocol is not known. */
        ProtocolInvalidOperationError,     /*! The requested operation is invalid for this protocol. */
        UnknownNetworkError,               /*! An unknown network-related error was detected. */
        UnknownProxyError,                 /*! An unknown proxy-related error was detected. */
        UnknownContentError,               /*! An breakdown in protocol was detected. */
        ProtocolFailure                    /*! A breakdown in protocol was detected. */
    };

    /*!
        Disposes of this error and frees any resources associated with it.
    */
    virtual ~NetworkError();

    /*!
        /fn ErrorCode code()

        Returns the code that represents the occurred error.
    */
    ErrorCode code();

    /*!
        /fn QString phrase()

        Returns a human readable explanation of the occurred error.
    */
    QString phrase();

    /*!
        /fn QString errorString() override

        Returns a string representation of the error.
    */
    QString errorString() override;

 protected:

    /*!
        Creates a new error from the provided http error struct.
        \internal
    */
    NetworkError(Transfers::Errors::NetworkErrorStruct err, QObject* parent);

 private:
    /*!
        Use pimpl pattern so that users do not have to be recompiled
        \internal
    */
    NetworkErrorPrivate* d_ptr;
};

class ProcessErrorPrivate;

/*!
    \class ProcessError 
    \brief The ProcessError represents an error that occurred during the
           post processing of a downloaded file.
    \since 0.3
*/
class ProcessError : public Error {
    Q_OBJECT
    Q_DECLARE_PRIVATE(ProcessError)

    friend class DownloadImpl;

 public:

    /*!
        Disposes of this error and frees any resources associated with it.
    */
    virtual ~ProcessError();

    /*!
        /fn QProcess::ProcessError code()

        Returns the code of the process error.
    */
    QProcess::ProcessError code();

    /*!
        /fn QString phrase()

        Returns a human readable explanation of the occurred process error.
    */
    QString phrase();

    /*!
        /fn int exitCode()

        Returns the exit code of the executed process.
    */
    int exitCode();

    /*!
        /fn QString standardOut()

        Returns the standard output of the executed process.
    */
    QString standardOut();

    /*!
        /fn QString standardError()

        Returns the standard error of the executed process.
    */
    QString standardError();

    /*!
        /fn QString errorString() override

        Returns a string representation of the error.
    */
    QString errorString() override;

 protected:

    /*!
        Creates a new error from the provided process error struct.
        \internal
    */
    ProcessError(Transfers::Errors::ProcessErrorStruct err, QObject* parent);

 private:
    /*!
        Use pimpl pattern so that users do not have to be recompiled
        \internal
    */
    ProcessErrorPrivate* d_ptr;
};

class HashErrorPrivate;

/*!
    \class HashError 
    \brief The HashError represents an error that occurred during the
           hash validation after the download.
    \since 0.3
*/
class HashError : public Error {
    Q_OBJECT
    Q_DECLARE_PRIVATE(HashError)

    friend class DownloadImpl;

 public:

    /*!
        Disposes of this error and frees any resources associated with it.
    */
    virtual ~HashError();

    /*!
        /fn QString method()

        Returns the method that was used to verify the checksum of the download.
     */
    QString method();

    /*!
        /fn QString expected()

        Returns the checksum that passed to the download manager to check against once the download
        was completed.
     */
    QString expected();

    /*!
        /fn QString checksum()

        Returns the checksum of the download on disk after the download was completed.
     */
    QString checksum();

    /*!
        /fn QString errorString() override

        Returns a string representation of the error.
    */
    QString errorString() override;

 protected:

    /*!
        Creates a new error from the provided process error struct.
        \internal
    */
    HashError(Transfers::Errors::HashErrorStruct err, QObject* parent);

 private:
    /*!
        Use pimpl pattern so that users do not have to be recompiled
        \internal
    */
    HashErrorPrivate* d_ptr;
};

}  // DownloadManager

}  // Ubuntu

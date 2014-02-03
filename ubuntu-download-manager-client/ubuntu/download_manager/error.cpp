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

#include <QDBusError>
#include <ubuntu/download_manager/metatypes.h>
#include "error.h"

namespace Ubuntu {

namespace DownloadManager {

/*
 * PRIMATE IMPLEMENTATIONS
 */

class ErrorPrivate {
    Q_DECLARE_PUBLIC(Error)

 public:
    ErrorPrivate(Error::Type type, Error* parent)
        : _type(type),
          q_ptr(parent) {
    }

    Error::Type type() {
        return _type;
    }

 private:
    Error::Type _type;
    Error* q_ptr;
};

class DBusErrorPrivate {
    Q_DECLARE_PUBLIC(DBusError)

 public:
    DBusErrorPrivate(QDBusError err, DBusError* parent)
        : _err(err),
          q_ptr(parent) {
    }

 private:
    QDBusError _err;
    DBusError* q_ptr;
};

class HttpErrorPrivate {
    Q_DECLARE_PUBLIC(HttpError)

 public:
    HttpErrorPrivate(HttpErrorStruct err, HttpError* parent)
        : _err(err),
          q_ptr(parent) {
    }

    inline int code() {
        return _err.getCode();
    }

    inline QString phrase() {
        return _err.getPhrase();
    }

 private:
    HttpErrorStruct _err;
    HttpError* q_ptr;
};

class NetworkErrorPrivate {
    Q_DECLARE_PUBLIC(NetworkError)

 public:
    NetworkErrorPrivate(NetworkErrorStruct err, NetworkError* parent)
        : _err(err),
          q_ptr(parent) {
    }

    NetworkError::ErrorCode code() {
        auto intCode = static_cast<NetworkError::ErrorCode>(_err.getCode());
        return intCode;
    }

    QString phrase() {
        return _err.getPhrase();
    }

 private:
    NetworkErrorStruct _err;
    NetworkError* q_ptr;
};

class ProcessErrorPrivate {
    Q_DECLARE_PUBLIC(ProcessError)

 public:
    ProcessErrorPrivate(ProcessErrorStruct err, ProcessError* parent)
        : _err(err),
          q_ptr(parent) {
    }

    QProcess::ProcessError code() {
        auto code = static_cast<QProcess::ProcessError>(_err.getCode());
        return code;
    }

    QString phrase() {
        return _err.getPhrase();
    }

    inline int exitCode() {
        return _err.getExitCode();
    }

    inline QString standardOut() {
        return _err.getStandardOutput();
    }

    inline QString standardError() {
        return _err.getStandardError();
    }

 private:
    ProcessErrorStruct _err;
    ProcessError* q_ptr;
};

/*
 * PUBLIC IMPLEMENTATIONS
 */

Error::Error(Error::Type type, QObject* parent)
    : QObject(parent),
      d_ptr(new ErrorPrivate(type, this)) {
}

Error::~Error() {
    delete d_ptr;
}

Error::Type
Error::type() {
    Q_D(Error);
    return d->type();
}


DBusError::DBusError(QDBusError err, QObject* parent)
    : Error(Error::DBus, parent),
      d_ptr(new DBusErrorPrivate(err, this)) {
}

DBusError::~DBusError() {
    delete d_ptr;
}

HttpError::HttpError(HttpErrorStruct err, QObject* parent)
    : Error(Error::Http, parent),
      d_ptr(new HttpErrorPrivate(err, this)) {
}

HttpError::~HttpError() {
    delete d_ptr;
}

int
HttpError::code() {
    Q_D(HttpError);
    return d->code();
}

QString
HttpError::phrase() {
    Q_D(HttpError);
    return d->phrase();
}

NetworkError::NetworkError(NetworkErrorStruct err, QObject* parent)
    : Error(Error::Network, parent),
      d_ptr(new NetworkErrorPrivate(err, this)) {
}

NetworkError::~NetworkError() {
    delete d_ptr;
}

NetworkError::ErrorCode
NetworkError::code() {
    Q_D(NetworkError);
    return d->code();
}

QString
NetworkError::phrase() {
    Q_D(NetworkError);
    return d->phrase();
}

ProcessError::ProcessError(ProcessErrorStruct err, QObject* parent)
    : Error(Error::Process, parent),
      d_ptr(new ProcessErrorPrivate(err, this)) {
}

ProcessError::~ProcessError() {
    delete d_ptr;
}

QString
ProcessError::phrase() {
    Q_D(ProcessError);
    return d->phrase();
}

QProcess::ProcessError
ProcessError::code() {
    Q_D(ProcessError);
    return d->code();
}

int
ProcessError::exitCode() {
    Q_D(ProcessError);
    return d->exitCode();
}

QString
ProcessError::standardOut() {
    Q_D(ProcessError);
    return d->standardOut();
}

QString
ProcessError::standardError() {
    Q_D(ProcessError);
    return d->standardError();
}

}  // DownloadManager

}  // Ubuntu

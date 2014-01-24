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

#include <QtDBus/QDBusConnection>
#include <QSharedPointer>
#include <QSslCertificate>
#include "downloads/daemon.h"
#include "downloads/manager.h"
#include "downloads/download_manager_adaptor.h"
#include "system/application.h"
#include "system/logger.h"

#define DISABLE_TIMEOUT "-disable-timeout"
#define SELFSIGNED_CERT "-self-signed-certs"
#define STOPPABLE "-stoppable"

namespace Ubuntu {

namespace DownloadManager {

/**
 * PRIVATE IMPLEMENATION
 */

class DaemonPrivate {
    Q_DECLARE_PUBLIC(Daemon)

 public:
    explicit DaemonPrivate(Daemon* parent)
        : q_ptr(parent) {
        _app = new Application();
        _conn = new DBusConnection();
        _shutDownTimer = new Timer();
        init();
    }

    DaemonPrivate(Application* app,
                  DBusConnection* conn,
                  Timer* timer,
                  Manager* man,
                  Daemon* parent)
        : _app(app),
          _shutDownTimer(timer),
          _conn(conn),
          _downInterface(man),
          q_ptr(parent) {
        init();
    }

    ~DaemonPrivate() {
        // no need to delete the adaptor because the interface is its parent
        delete _app;
        delete _conn;
        delete _downInterface;
        delete _shutDownTimer;
    }

    bool isTimeoutEnabled() {
        return _isTimeoutEnabled;
    }

    void enableTimeout(bool enabled) {
        _isTimeoutEnabled = enabled;
    }

    bool isStoppable() {
        return _stoppable;
    }

    void setStoppable(bool stoppable) {
        _stoppable = stoppable;
    }

    QList<QSslCertificate> selfSignedCerts() {
        return _certs;
    }

    void setSelfSignedCerts(QList<QSslCertificate> certs) {
        _certs = certs;
    }

    void start(QString path) {
        DLOG(INFO) << " " << __PRETTY_FUNCTION__;
        _downAdaptor = new DownloadManagerAdaptor(_downInterface);
        bool ret = _conn->registerService(path);
        if (ret) {
            LOG(INFO) << "Service registered to"
                << "com.canonical.applications.Downloader";
            ret = _conn->registerObject("/", _downInterface);
            if (!ret) {
                LOG(INFO) << "Could not register interface. DBus Error =>"
                    << _conn->connection().lastError();
                _app->exit(-1);
            }
            return;
        }
        LOG(INFO) << "Could not register service. DBus Error =>"
            << _conn->connection().lastError();
        _app->exit(-1);
    }

    void onTimeout() {
        LOG(INFO) << "Timeout reached, shutdown service.";
        _app->exit(0);
    }

    void onDownloadManagerSizeChanged(int size) {
        DLOG(INFO) << " " << __PRETTY_FUNCTION__ << size;
        bool isActive = _shutDownTimer->isActive();

        if (isActive && size > 0) {
            LOG(INFO) << "Timer must be stopped because we have " << size
                << "downloads.";
            _shutDownTimer->stop();
        }
        if (!isActive && size == 0) {
            LOG(INFO) << "Timer must be started because we have 0 downloads.";
            _shutDownTimer->start(timeout);
        }
    }

    static const int timeout = 30000;

 private:
    void parseCommandLine() {
        QStringList args = _app->arguments();
        if (args.contains(SELFSIGNED_CERT)) {
            int index = args.indexOf(SELFSIGNED_CERT);
            if (args.count() > index + 1) {
                QString certsRegex = args[index + 1];
                _certs = QSslCertificate::fromPath(certsRegex);
                LOG(INFO) << "Accepting self signed certs at path"
                    << certsRegex;
            } else {
                LOG(INFO) << "Missing certs path.";
            }
        }  // certs
        _isTimeoutEnabled = !args.contains(DISABLE_TIMEOUT);
        LOG(INFO) << "Timeout is enabled: " << _isTimeoutEnabled;
        _stoppable = args.contains(STOPPABLE);
        LOG(INFO) << "Daemon is stoppable: " << _stoppable;
    }

    void init() {
        Q_Q(Daemon);

        // parse command line to decide if the timer is enabled and if
        // we accept self signed certs
        parseCommandLine();
        if (_isTimeoutEnabled) {
            q->connect(_shutDownTimer, SIGNAL(timeout()),
                q, SLOT(onTimeout()));
            _shutDownTimer->start(timeout);
        }

        if (_downInterface == NULL) {
            _downInterface = new Manager(_app, _conn, _stoppable,
                q_ptr);
        }

        _downInterface->setAcceptedCertificates(_certs);
        // connect to the download manager changes
        q->connect(_downInterface,
            SIGNAL(sizeChanged(int)),  // NOLINT (readability/function)
            q, SLOT(onDownloadManagerSizeChanged(int))); // NOLINT (readability/function)

        // set logging
        Logger::setupLogging();
    }

 private:
    bool _isTimeoutEnabled = true;
    bool _stoppable = false;
    QList<QSslCertificate> _certs;
    Application* _app;
    Timer* _shutDownTimer;
    DBusConnection* _conn;
    Manager* _downInterface;
    DownloadManagerAdaptor* _downAdaptor;
    Daemon* q_ptr;
};

/**
 * PUBLIC IMPLEMENTATION
 */

Daemon::Daemon(QObject *parent)
    : QObject(parent),
      d_ptr(new DaemonPrivate(this)) {
}

Daemon::Daemon(Application* app,
               DBusConnection* conn,
               Timer* timer,
               Manager* man,
               QObject *parent)
    : QObject(parent),
      d_ptr(new DaemonPrivate(app, conn, timer, man, this)) {
}

Daemon::~Daemon() {
    delete d_ptr;
}

bool
Daemon::isTimeoutEnabled() {
    Q_D(Daemon);
    return d->isTimeoutEnabled();
}

void
Daemon::enableTimeout(bool enabled) {
    Q_D(Daemon);
    d->enableTimeout(enabled);
}

bool
Daemon::isStoppable() {
    Q_D(Daemon);
    return d->isStoppable();
}

void
Daemon::setStoppable(bool stoppable) {
    Q_D(Daemon);
    d->setStoppable(stoppable);
}

QList<QSslCertificate>
Daemon::selfSignedCerts() {
    Q_D(Daemon);
    return d->selfSignedCerts();
}

void
Daemon::setSelfSignedCerts(QList<QSslCertificate> certs) {
    Q_D(Daemon);
    d->setSelfSignedCerts(certs);
}

void
Daemon::start(QString path) {
    Q_D(Daemon);
    d->start(path);
}

}  // DownloadManager

}  // Ubuntu

#include "moc_daemon.cpp"

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
#include <QDebug>
#include <QSharedPointer>
#include <QSslCertificate>
#include "./application.h"
#include "./logger.h"
#include "./download_manager.h"
#include "./download_manager_adaptor.h"
#include "./download_daemon.h"

#define DISABPLE_TIMEOUT "-disable-timeout"
#define SELFSIGNED_CERT "-self-signed-certs"

/**
 * PRIVATE IMPLEMENATION
 */

class DownloadDaemonPrivate {
    Q_DECLARE_PUBLIC(DownloadDaemon)

 public:
    explicit DownloadDaemonPrivate(DownloadDaemon* parent)
        : q_ptr(parent) {
        _app = new Application();
        _conn = QSharedPointer<DBusConnection>(new DBusConnection());
        _shutDownTimer = new Timer();
        _downInterface = new DownloadManager(_conn, q_ptr);
        init();
    }

    DownloadDaemonPrivate(Application* app,
                          DBusConnection* conn,
                          Timer* timer,
                          DownloadManager* man,
                          DownloadDaemon* parent)
        : _app(app),
          _shutDownTimer(timer),
          _conn(conn),
          _downInterface(man),
          q_ptr(parent) {
        init();
    }

    ~DownloadDaemonPrivate() {
        // no need to delete the adaptor because the interface is its parent
        if (_downInterface)
            delete _downInterface;
        if (_app)
            delete _app;
        if (_shutDownTimer)
            delete _shutDownTimer;

        // stop logging
        Logger::stopLogging();
    }

    void start() {
        qDebug() << "Starting daemon";
        _downAdaptor = new DownloadManagerAdaptor(_downInterface);
        bool ret = _conn->registerService(
            "com.canonical.applications.Downloader");
        if (ret) {
            qDebug() << "Service registered to"
                << "com.canonical.applications.Downloader";
            ret = _conn->registerObject("/", _downInterface);
            qDebug() << ret;
            if (!ret) {
                qDebug() << "Could not register interface";
                _app->exit(-1);
            }
            return;
        }
        qDebug() << "Could not register service";
        _app->exit(-1);
    }

    void onTimeout() {
        qDebug() << "Timeout reached, shutdown service.";
        _app->exit(0);
    }

    void onDownloadManagerSizeChanged(int size) {
        bool isActive = _shutDownTimer->isActive();
        qDebug() << "Timer is active:" << isActive << "size is:" << size;

        if (isActive && size > 0) {
            qDebug() << "Timer must be stopped because we have" << size
                << "downloads.";
            _shutDownTimer->stop();
        }
        if (!isActive && size == 0) {
            qDebug() << "Timer must be started because we have 0 downloads.";
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
                qDebug() << "Accepting self signed certs" << _certs;
            } else {
                qCritical() << "Missing certs path";
            }
        }  // certs
        _isTimeoutEnabled = !args.contains(DISABPLE_TIMEOUT);
        qDebug() << "Timeout is enabled:" << _isTimeoutEnabled;
    }

    void init() {
        Q_Q(DownloadDaemon);

        // parse command line to decide if the timer is enabled and if
        // we accept self signed certs
        parseCommandLine();
        if (_isTimeoutEnabled) {
            q->connect(_shutDownTimer, SIGNAL(timeout()),
                q, SLOT(onTimeout()));
            _shutDownTimer->start(timeout);
        }

        _downInterface->setAcceptedCertificates(_certs);
        // connect to the download manager changes
        q->connect(_downInterface,
            SIGNAL(sizeChanged(int)),  // NOLINT (readability/function)
            q, SLOT(onDownloadManagerSizeChanged(int))); // NOLINT (readability/function)

        // set logging
        Logger::setupLogging();
#ifdef DEBUG
        Logger::setLogLevel(QtDebugMsg);
#else
        if (qgetenv("UBUNTU_DOWNLOADER_DEBUG") != "")
            Logger::setLogLevel(QtDebugMsg);
#endif
    }

 private:
    bool _isTimeoutEnabled;
    QList<QSslCertificate> _certs;
    Application* _app;
    Timer* _shutDownTimer;
    QSharedPointer<DBusConnection> _conn;
    DownloadManager* _downInterface;
    DownloadManagerAdaptor* _downAdaptor;
    DownloadDaemon* q_ptr;
};

/**
 * PUBLIC IMPLEMENTATION
 */

DownloadDaemon::DownloadDaemon(QObject *parent)
    : QObject(parent),
      d_ptr(new DownloadDaemonPrivate(this)) {
}

DownloadDaemon::DownloadDaemon(Application* app,
                               DBusConnection* conn,
                               Timer* timer,
                               DownloadManager* man,
                               QObject *parent)
    : QObject(parent),
      d_ptr(new DownloadDaemonPrivate(app, conn, timer, man, this)) {
}

void
DownloadDaemon::start() {
    Q_D(DownloadDaemon);
    d->start();
}

#include "moc_download_daemon.cpp"

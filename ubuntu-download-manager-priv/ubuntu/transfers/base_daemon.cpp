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

#include <QDebug>
#include <QtDBus/QDBusConnection>
#include <QSharedPointer>
#include <QSslCertificate>
#include "ubuntu/transfers/system/application.h"
#include "ubuntu/transfers/system/logger.h"
#include "ubuntu/transfers/system/timer.h"
#include "adaptor_factory.h"
#include "manager_factory.h"
#include "base_daemon.h"


namespace {
    const QString DISABLE_TIMEOUT = "-disable-timeout";
    const QString SELFSIGNED_CERT = "-self-signed-certs";
    const QString STOPPABLE =  "-stoppable";
    const QString LOG_DIR= "-log-dir";
    const int DEFAULT_TIMEOUT = 30000;
}

namespace Ubuntu {

using namespace System;

namespace Transfers {

BaseDaemon::BaseDaemon(ManagerFactory* managerFactory,
               AdaptorFactory* adaptorFactory,
               QObject *parent)
    : QObject(parent),
      _managerFactory(managerFactory),
      _adaptorFactory(adaptorFactory) {

    _app = new Application();
    _conn = new DBusConnection();
    _shutDownTimer = new Timer();
    init();
}

BaseDaemon::BaseDaemon(ManagerFactory* managerFactory,
               AdaptorFactory* adaptorFactory,
               Application* app,
               DBusConnection* conn,
               Timer* timer,
               QObject* parent)
    : QObject(parent),
      _app(app),
      _shutDownTimer(timer),
      _conn(conn),
      _managerFactory(managerFactory),
      _adaptorFactory(adaptorFactory) {
    init();
}

BaseDaemon::~BaseDaemon() {
    // no need to delete the adaptor because the interface is its parent
    delete _app;
    delete _conn;
    delete _manager;
    delete _shutDownTimer;
    delete _managerFactory;
    delete _adaptorFactory;
}

bool
BaseDaemon::isTimeoutEnabled() {
    return _isTimeoutEnabled;
}

void
BaseDaemon::enableTimeout(bool enabled) {
    _isTimeoutEnabled = enabled;
    if (!_isTimeoutEnabled) {
        _shutDownTimer->stop();
    }
}

bool
BaseDaemon::isStoppable() {
    return _stoppable;
}

void
BaseDaemon::setStoppable(bool stoppable) {
    _stoppable = stoppable;
}

QList<QSslCertificate>
BaseDaemon::selfSignedCerts() {
    return _certs;
}

void
BaseDaemon::setSelfSignedCerts(QList<QSslCertificate> certs) {
    _certs = certs;
}

void
BaseDaemon::start(const QString& path) {
    TRACE;
    _path = path;
    _managerAdaptor = _adaptorFactory->createAdaptor(_manager);
    bool ret = _conn->registerService(_path);
    if (ret) {
        LOG(INFO) << "Service registered to" << _path;
        ret = _conn->registerObject("/", _manager);
        if (!ret) {
            LOG(ERROR) << "Could not register interface. DBus Error =>"
                << _conn->connection().lastError();
            _app->exit(-1);
        }
        return;
    }
    LOG(ERROR) << "Could not register service. DBus Error =>"
        << _conn->connection().lastError();
    _app->exit(-1);
}

void
BaseDaemon::stop() {
    // stop listening in the service
    bool ret = _conn->unregisterService(_path);
    if (!ret) {
        qCritical() << "Could not unregister service at" << _path;
    }
}

BaseManager*
BaseDaemon::manager() {
    return _manager;
}

void
BaseDaemon::onTimeout() {
    LOG(INFO) << "Timeout reached, shutdown service.";
    _app->exit(0);
}

void
BaseDaemon::onDownloadManagerSizeChanged(int size) {
    TRACE << size;
    bool isActive = _shutDownTimer->isActive();

    if (isActive && size > 0) {
        LOG(INFO) << "Timer must be stopped because we have " << size
            << "downloads.";
        _shutDownTimer->stop();
    }
    if (!isActive && size == 0) {
        LOG(INFO) << "Timer must be started because we have 0 downloads.";
        _shutDownTimer->start(DEFAULT_TIMEOUT);
    }
}

void
BaseDaemon::parseCommandLine() {
    QStringList args = _app->arguments();
    int index;

    // set logging
    if (args.contains(LOG_DIR)) {
        index = args.indexOf(LOG_DIR);
        if (args.count() > index + 1) {
            auto logPath = args[index + 1];
            Logger::setupLogging(logPath);
            LOG(INFO) << "Log path is" << logPath;
        } else {
            LOG(ERROR) << "Missing log dir path.";
            Logger::setupLogging();
        }
    } else {
        Logger::setupLogging();
    }

    if (args.contains(SELFSIGNED_CERT)) {
        index = args.indexOf(SELFSIGNED_CERT);
        if (args.count() > index + 1) {
            QString certsRegex = args[index + 1];
            _certs = QSslCertificate::fromPath(certsRegex);
            LOG(INFO) << "Accepting self signed certs at path"
                << certsRegex;
        } else {
            LOG(ERROR) << "Missing certs path.";
        }
    }  // certs
    _isTimeoutEnabled = !args.contains(DISABLE_TIMEOUT);
    LOG(INFO) << "Timeout is enabled: " << _isTimeoutEnabled;
    _stoppable = args.contains(STOPPABLE);
    LOG(INFO) << "Daemon is stoppable: " << _stoppable;
}

void
BaseDaemon::init() {
    // parse command line to decide if the timer is enabled and if
    // we accept self signed certs
    parseCommandLine();
    if (_isTimeoutEnabled) {
        connect(_shutDownTimer, &Timer::timeout,
            this, &BaseDaemon::onTimeout);
        _shutDownTimer->start(DEFAULT_TIMEOUT);
    }

    _manager = _managerFactory->createManager(
        _app, _conn, _stoppable, this);

    _manager->setAcceptedCertificates(_certs);
    // connect to the download manager changes
    connect(_manager,
        &BaseManager::sizeChanged,  // NOLINT (readability/function)
        this, &BaseDaemon::onDownloadManagerSizeChanged); // NOLINT (readability/function)
}

}  // General

}  // Ubuntu

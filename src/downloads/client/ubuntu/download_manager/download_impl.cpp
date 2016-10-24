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

#include <QProcessEnvironment>
#include <ubuntu/download_manager/logging/logger.h>
#include "download_impl.h"

namespace {
    const QString CLICK_PACKAGE_PROPERTY = "ClickPackage";
    const QString SHOW_INDICATOR_PROPERTY = "ShowInIndicator";
    const QString TITLE_PROPERTY = "Title";
}

namespace Ubuntu {

namespace DownloadManager {

using namespace Logging;

DownloadImpl::DownloadImpl(const QDBusConnection& conn,
                           const QString& servicePath,
                           const QDBusObjectPath& objectPath,
                           QObject* parent)
    : Download(parent),
      _id(objectPath.path()),
      _conn(conn),
      _servicePath(servicePath) {

    _dbusInterface = new DownloadInterface(servicePath,
        _id, conn);

    _propertiesInterface = new PropertiesInterface(servicePath,
        _id, conn);

    // fwd all the signals but the error one
    auto connected = connect(_dbusInterface, &DownloadInterface::canceled,
        this, &Download::canceled);

    if (!connected) {
        Logger::log(Logger::Critical,
            "Could not connect to signal DownloadInterface::canceled");
    }

    connected = connect(_dbusInterface, &DownloadInterface::finished,
        this, &Download::finished);
    if (!connected) {
        Logger::log(Logger::Critical,
            "Could not connect to signal &DownloadInterface::finished");
    }

    connected = connect(_dbusInterface, &DownloadInterface::finished,
        this, &DownloadImpl::onFinished);
    if (!connected) {
        Logger::log(Logger::Critical,
            "Could not connect to signal &DownloadInterface::finished");
    }

    connected = connect(_dbusInterface, &DownloadInterface::paused,
        this, &Download::paused);
    if (!connected) {
        Logger::log(Logger::Critical,
            "Could not connect to signal DownloadInterface::paused");
    }

    connected = connect(_dbusInterface, &DownloadInterface::processing,
        this, &Download::processing);
    if (!connected) {
        Logger::log(Logger::Critical,
            "Could not connect to signal DownloadInterface::processing");
    }

    connected = connect(_dbusInterface, static_cast<void(DownloadInterface::*)
        (qulonglong, qulonglong)>(&DownloadInterface::progress),
        this, static_cast<void(Download::*)
            (qulonglong, qulonglong)>(&Download::progress));
    if (!connected) {
        Logger::log(Logger::Critical,
            "Could not connect to signal &DownloadInterface::progress");
    }

    connected = connect(_dbusInterface, &DownloadInterface::resumed,
        this, &Download::resumed);
    if (!connected) {
        Logger::log(Logger::Critical,
            "Could not connect to signal &DownloadInterface::resumed");
    }

    connected = connect(_dbusInterface, &DownloadInterface::started,
        this, &Download::started);
    if (!connected) {
        Logger::log(Logger::Critical,
            "Could not connect to signal &DownloadInterface::started");
    }

    // connect to the different type of errors that will later be converted to
    // the error type to be used by the client.
    connected = connect(_dbusInterface, &DownloadInterface::httpError,
        this, &DownloadImpl::onHttpError);
    if (!connected) {
        Logger::log(Logger::Critical,
            "Could not connect to signal &DownloadInterface::httpError");
    }

    connected = connect(_dbusInterface, &DownloadInterface::networkError,
        this, &DownloadImpl::onNetworkError);
    if (!connected) {
        Logger::log(Logger::Critical,
            "Could not connect to signal &DownloadInterface::networkError");
    }

    connected = connect(_dbusInterface, &DownloadInterface::processError,
        this, &DownloadImpl::onProcessError);
    if (!connected) {
        Logger::log(Logger::Critical,
            "Could not connect to signal &DownloadInterface::processError");
    }

    connected = connect(_dbusInterface, &DownloadInterface::authError,
        this, &DownloadImpl::onAuthError);
    if (!connected) {
        Logger::log(Logger::Critical,
            "Could not connect to signal &DownloadInterface::authError");
    }

    connected = connect(_dbusInterface, &DownloadInterface::hashError,
        this, &DownloadImpl::onHashError);
    if (!connected) {
        Logger::log(Logger::Critical,
            "Could not connect to signal &DownloadInterface::authError");
    }

    connected = connect(_propertiesInterface, &PropertiesInterface::PropertiesChanged,
        this, &DownloadImpl::onPropertiesChanged);
    if (!connected) {
        Logger::log(Logger::Critical,
            "Could not connect to signal &PropertiesInterface::PropertiesChanged");
    }
}

DownloadImpl::DownloadImpl(const QDBusConnection& conn, Error* err, QObject* parent)
    : Download(parent),
      _isError(true),
      _lastError(err),
      _conn(conn) {
}

DownloadImpl::~DownloadImpl() {
    delete _lastError;
    delete _dbusInterface;
    delete _propertiesInterface;
}

void
DownloadImpl::setLastError(Error* err) {
    Logger::log(Logger::Debug,
        QString("Download{%1} setLastError(%2)").arg(_id).arg(
            err->errorString()));
    if (_lastError != nullptr) {
        delete _lastError;
    }
    _lastError = err;
    _isError = true;
    emit Download::error(err);
}

void
DownloadImpl::setLastError(const QDBusError& err) {
    setLastError(new DBusError(err, this));
}

void
DownloadImpl::start() {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return;
    }
    Logger::log(Logger::Debug, QString("Download{%1} start())").arg(_id));
    QDBusPendingCall call =
        _dbusInterface->start();
    auto watcher = new DownloadPCW(_conn, _servicePath,
        call, this);
    Q_UNUSED(watcher);
}

void
DownloadImpl::pause() {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return;
    }
    Logger::log(Logger::Debug, QString("Download{%1} pause())").arg(_id));
    QDBusPendingCall call =
        _dbusInterface->pause();
    auto watcher = new DownloadPCW(_conn, _servicePath,
        call, this);
    Q_UNUSED(watcher);
}

void
DownloadImpl::resume() {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return;
    }
    Logger::log(Logger::Debug, QString("Download{%1} resume())").arg(_id));
    QDBusPendingCall call =
        _dbusInterface->resume();
    auto watcher = new DownloadPCW(_conn, _servicePath,
        call, this);
    Q_UNUSED(watcher);
}

void
DownloadImpl::cancel() {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return;
    }
    Logger::log(Logger::Debug, QString("Download{%1} cancel())").arg(_id));
    QDBusPendingCall call =
        _dbusInterface->cancel();
    auto watcher = new DownloadPCW(_conn, _servicePath,
        call, this);
    Q_UNUSED(watcher);
}

void
DownloadImpl::collected() {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return;
    }
    Logger::log(Logger::Debug, QString("Download{%1} collected()").arg(_id));
    QDBusPendingReply<> reply =
        _dbusInterface->collected();
    // block, the call should be fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        Logger::log(Logger::Error, "Error when setting download collected");
        setLastError(reply.error());
    }
}

void
DownloadImpl::allowMobileDownload(bool allowed) {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return;
    }
    Logger::log(Logger::Debug,
        QString("Download{%1} allowMobileDownload%2())").arg(_id).arg(allowed));
    QDBusPendingReply<> reply =
        _dbusInterface->allowGSMDownload(allowed);
    // block, the call should be fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        Logger::log(Logger::Error, "Error when setting mobile data usage");
        setLastError(reply.error());
    }
}

bool
DownloadImpl::isMobileDownloadAllowed() {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return false;
    }
    Logger::log(Logger::Debug,
        QString("Download{%1} isMobileDownloadAllowed").arg(_id));
    QDBusPendingReply<bool> reply =
        _dbusInterface->isGSMDownloadAllowed();
    // block, the call should be fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        Logger::log(Logger::Error, "Error when querying mobile data usage");
        setLastError(reply.error());
        return false;
    } else {
        auto result = reply.value();
        return result;
    }
}

void
DownloadImpl::setDestinationDir(const QString& path) {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return;
    }
    Logger::log(Logger::Debug, QString("Dowmload{%1} setDestinationDir(%2)")
        .arg(_id).arg(path));
    QDBusPendingReply<> reply =
        _dbusInterface->setDestinationDir(path);
    // block, the call should be fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        Logger::log(Logger::Error, "Error setting the download directory");
        setLastError(reply.error());
    }
}

void
DownloadImpl::setHeaders(QMap<QString, QString> headers) {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return;
    }
    Logger::log(Logger::Debug,
        QString("Download {%1} setHeaders(%2)").arg(_id), headers);

    QDBusPendingReply<> reply =
        _dbusInterface->setHeaders(headers);
    // block, the call should be fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        Logger::log(Logger::Error, "Error setting the download headers");
        setLastError(reply.error());
    }
}

QVariantMap
DownloadImpl::metadata() {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        QVariantMap emptyResult;
        return emptyResult;
    }
    Logger::log(Logger::Debug, QString("Download{%1} metadata()").arg(_id));
    QDBusPendingReply<QVariantMap> reply =
        _dbusInterface->metadata();
    // block the call is fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        Logger::log(Logger::Error, "Error querying the download metadata");
        QVariantMap emptyResult;
        setLastError(reply.error());
        return emptyResult;
    } else {
        auto result = reply.value();
        return result;
    }
}

void
DownloadImpl::setMetadata(QVariantMap map) {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return;
    }
    Logger::log(Logger::Debug,
        QString("Download {%1} setMetadata(%2)").arg(_id), map);

    QDBusPendingReply<> reply =
        _dbusInterface->setMetadata(map);
    // block, the call should be fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        Logger::log(Logger::Error, "Error setting the download metadata");
        setLastError(reply.error());
    }
}

QMap<QString, QString>
DownloadImpl::headers() {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        QMap<QString, QString> empty;
        return empty;
    }
    Logger::log(Logger::Debug, QString("Download{%1} headers()").arg(_id));
    QDBusPendingReply<QMap<QString, QString> > reply =
        _dbusInterface->headers();
    // block, the call should be fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        Logger::log(Logger::Error, "Error querying the download headers");
        setLastError(reply.error());
        QMap<QString, QString> empty;
        return empty;
    } else {
        auto result = reply.value();
        return result;
    }
}


void
DownloadImpl::setThrottle(qulonglong speed) {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return;
    }
    Logger::log(Logger::Debug,
        QString("Download{%1} setThrottle(%2)").arg(_id).arg(speed));
    QDBusPendingReply<> reply =
        _dbusInterface->setThrottle(speed);
    // block, the call should be fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        Logger::log(Logger::Error, "Error setting the download throttle");
        setLastError(reply.error());
    }
}

qulonglong
DownloadImpl::throttle() {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return 0;
    }
    Logger::log(Logger::Debug, QString("Download{%1} throttle()").arg(_id));
    QDBusPendingReply<qulonglong> reply =
        _dbusInterface->throttle();
    // block, the call is fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        Logger::log(Logger::Error, "Error querying the download throttle");
        setLastError(reply.error());
        return 0;
    } else {
        auto result = reply.value();
        return result;
    }
}

QString
DownloadImpl::filePath() {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return "";
    }
    Logger::log(Logger::Debug, QString("Download{%1} filePath()").arg(_id));
    QDBusPendingReply<QString> reply =
        _dbusInterface->filePath();
    // block, the call is fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        Logger::log(Logger::Error, "Error querying the download file path");
        setLastError(reply.error());
        return "";
    } else {
        auto result = reply.value();
        return result;
    }
}

Download::State
DownloadImpl::state() {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return Download::ERROR;
    }
    Logger::log(Logger::Debug, QString("Download{%1} state()").arg(_id));
    QDBusPendingReply<int> reply =
        _dbusInterface->state();
    // block, the call is fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        Logger::log(Logger::Error, "Error querying the download state");
        setLastError(reply.error());
        return Download::ERROR;
    } else {
        auto result = static_cast<Download::State>(reply.value());
        return result;
    }
}

QString
DownloadImpl::id() const {
    return _id;
}

qulonglong
DownloadImpl::progress() {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return 0;
    }
    Logger::log(Logger::Debug, QString("Download{%1} progress()").arg(_id));
    QDBusPendingReply<qulonglong> reply =
        _dbusInterface->progress();
    // block call should be fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        Logger::log(Logger::Error, "Error querying the download progress");
        setLastError(reply.error());
        return 0;
    } else {
        auto result = reply.value();
        return result;
    }
}

qulonglong
DownloadImpl::totalSize() {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return 0;
    }
    Logger::log(Logger::Debug, QString("Download{%1} totalSize()").arg(_id));
    QDBusPendingReply<qulonglong> reply =
        _dbusInterface->totalSize();
    // block call should be fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        Logger::log(Logger::Error, "Error querying the download size");
        setLastError(reply.error());
        return 0;
    } else {
        auto result = reply.value();
        return result;
    }
}

bool
DownloadImpl::isError() const {
    return _isError;
}

Error*
DownloadImpl::error() const {
    return _lastError;
}

QString
DownloadImpl::clickPackage() const {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return "";
    }
    return _dbusInterface->clickPackage();
}

bool
DownloadImpl::showInIndicator() const {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return false;
    }
    return _dbusInterface->showInIndicator();
}

QString
DownloadImpl::title() const {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return "";
    }
    return _dbusInterface->title();
}

QString
DownloadImpl::destinationApp() const {
    if (_dbusInterface == nullptr || !_dbusInterface->isValid()) {
        Logger::log(Logger::Error, QString("Invalid dbus interface: %1").arg(_lastError->errorString()));
        return "";
    }
    return _dbusInterface->destinationApp();
}

void
DownloadImpl::onHttpError(HttpErrorStruct errStruct) {
    auto err = new HttpError(errStruct, this);
    setLastError(err);
}

void
DownloadImpl::onNetworkError(NetworkErrorStruct errStruct) {
    auto err = new NetworkError(errStruct, this);
    setLastError(err);
}

void
DownloadImpl::onProcessError(ProcessErrorStruct errStruct) {
    auto err = new ProcessError(errStruct, this);
    setLastError(err);
}

void
DownloadImpl::onAuthError(AuthErrorStruct errStruct) {
    auto err = new AuthError(errStruct, this);
    setLastError(err);
}

void
DownloadImpl::onHashError(HashErrorStruct errStruct) {
    auto err = new HashError(errStruct, this);
    setLastError(err);
}

void
DownloadImpl::onPropertiesChanged(const QString& interfaceName,
                                  const QVariantMap& changedProperties,
                                  const QStringList& invalidatedProperties) {
    Q_UNUSED(invalidatedProperties);
    // just take care of the property changes from the download interface
    if (interfaceName == DownloadInterface::staticInterfaceName()) {
        if (changedProperties.contains(CLICK_PACKAGE_PROPERTY)) {
            emit clickPackagedChanged();
        }

        if (changedProperties.contains(SHOW_INDICATOR_PROPERTY)) {
            emit showInIndicatorChanged();
        }

        if (changedProperties.contains(TITLE_PROPERTY)) {
            emit titleChanged();
        }
    }
}

void DownloadImpl::onFinished(const QString &path) {
    Q_UNUSED(path);

    // Only acknowledge collection automatically if we aren't sending 
    // this download to another app via content-hub
    auto environment = QProcessEnvironment::systemEnvironment();
    QString appId;
    if (environment.contains("APP_ID")) {
        appId = environment.value("APP_ID");
    } else {
        appId = QCoreApplication::applicationFilePath();
    }

    if (appId == metadata().value("app-id", appId)) {
        // Inform UDM that we've received the finished signal, so the download
        // can be considered completely finished.
        collected();
    }
}

}  // DownloadManager

}  // Ubuntu

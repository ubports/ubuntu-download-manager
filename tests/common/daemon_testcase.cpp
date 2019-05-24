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

#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QDBusMessage>
#include <QFile>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QScopedPointer>
#include <QThread>

#include "testing_interface.h"
#include "daemon_testcase.h"
#define TEST_DAEMON "ubuntu-download-manager-test-daemon"
#define LOCAL_HOST "http://127.0.0.1:%1"

DaemonTestCase::DaemonTestCase(const QString& testName,
                               QObject* parent)
    : BaseTestCase(testName, parent),
      _daemonExec(TEST_DAEMON) {
}

DaemonTestCase::DaemonTestCase(const QString& testName,
                               const QString& daemonProcess,
                               QObject* parent)
    : BaseTestCase(testName, parent),
      _daemonExec(daemonProcess) {
}


void
DaemonTestCase::addFileToHttpServer(const QString& absolutePath) {
    // copy a file to the locatin of the http server so that is served
    QFile serverFile(absolutePath);
    if (serverFile.exists()) {
        QFileInfo fileInfo(serverFile.fileName());
        QString filename(fileInfo.fileName());
        serverFile.copy(httpServerDir() + "/" + filename);
    } else {
        QSKIP(QString("File '%1' does not exist.").arg(absolutePath).toUtf8(), SkipSingle);
    }
}

QString
DaemonTestCase::daemonPath() {
    return _daemonPath;
}

QUrl
DaemonTestCase::serverUrl() {
    QString urlStr = QString(LOCAL_HOST).arg(_port);
    QUrl url(urlStr);
    return url;
}

void
DaemonTestCase::returnDBusErrors(bool errors) {
    if (_daemonProcess != nullptr) {
        auto conn = QDBusConnection::sessionBus();
        QScopedPointer<TestingInterface> testingInterface(new TestingInterface(
            _daemonPath, "/", conn));
        QDBusPendingReply<> reply =
            testingInterface->returnDBusErrors(errors);
        reply.waitForFinished();

        if (reply.isError()) {
	    auto error = reply.error();
	    QString msg = "Could not tell the daemon to return DBus errors: "
                + error.name() + ":" +  error.message();
            QSKIP(msg.toUtf8(), SkipSingle);
        }
    } else {
        QFAIL("returnDBusErrors must be used after init has been executed.");
    }
}

void
DaemonTestCase::returnAuthError(const QString &download, AuthErrorStruct error) {
    if (_daemonProcess != nullptr) {
        auto conn = QDBusConnection::sessionBus();
        QScopedPointer<TestingInterface> testingInterface(new TestingInterface(
            _daemonPath, "/", conn));
        QDBusPendingReply<> reply =
            testingInterface->returnAuthError(download, error);
        reply.waitForFinished();

        if (reply.isError()) {
	    auto error = reply.error();
	    QString msg = "Could not tell the daemon to return Auth errors: "
                + error.name() + ":" +  error.message();
            QSKIP(msg.toUtf8(), SkipSingle);
        }
    } else {
        QFAIL("returnAuthError must be used after init has been executed.");
    }
}

void
DaemonTestCase::returnHttpError(const QString &download, HttpErrorStruct error) {
    if (_daemonProcess != nullptr) {
        auto conn = QDBusConnection::sessionBus();
        QScopedPointer<TestingInterface> testingInterface(new TestingInterface(
            _daemonPath, "/", conn));
        QDBusPendingReply<> reply =
            testingInterface->returnHttpError(download, error);
        reply.waitForFinished();

        if (reply.isError()) {
	    auto error = reply.error();
	    QString msg = "Could not tell the daemon to return Http errors: "
                + error.name() + ":" +  error.message();
            QSKIP(msg.toUtf8(), SkipSingle);
        }
    } else {
        QFAIL("returnHttpError must be used after init has been executed.");
    }
}

void
DaemonTestCase::returnNetworkError(const QString &download,
                                   NetworkErrorStruct error) {
    if (_daemonProcess != nullptr) {
        auto conn = QDBusConnection::sessionBus();
        QScopedPointer<TestingInterface> testingInterface(new TestingInterface(
            _daemonPath, "/", conn));
        QDBusPendingReply<> reply =
            testingInterface->returnNetworkError(download, error);
        reply.waitForFinished();

        if (reply.isError()) {
	    auto error = reply.error();
	    QString msg = "Could not tell the daemon to return Network errors: "
                + error.name() + ":" +  error.message();
            QSKIP(msg.toUtf8(), SkipSingle);
        }
    } else {
        QFAIL("returnNetworkError must be used after init has been executed.");
    }
}

void
DaemonTestCase::returnProcessError(const QString &download,
                                   ProcessErrorStruct error) {
    if (_daemonProcess != nullptr) {
        auto conn = QDBusConnection::sessionBus();
        QScopedPointer<TestingInterface> testingInterface(new TestingInterface(
            _daemonPath, "/", conn));
        QDBusPendingReply<> reply =
             testingInterface->returnProcessError(download, error);
        reply.waitForFinished();

        if (reply.isError()) {
	    auto error = reply.error();
	    QString msg = "Could not tell the daemon to return Process errors: "
                + error.name() + ":" +  error.message();
            QSKIP(msg.toUtf8(), SkipSingle);
        }
    } else {
        QFAIL("returnProcessError must be used after init has been executed.");
    }
}

void
DaemonTestCase::returnHashError(const QString &download, HashErrorStruct error) {
    if (_daemonProcess != nullptr) {
        auto conn = QDBusConnection::sessionBus();
        QScopedPointer<TestingInterface> testingInterface(new TestingInterface(
                _daemonPath, "/", conn));
        QDBusPendingReply<> reply =
                testingInterface->returnHashError(download, error);
        reply.waitForFinished();

        if (reply.isError()) {
            auto error = reply.error();
            QString msg = "Could not tell the daemon to return Hash errors: "
                    + error.name() + ":" +  error.message();
            QSKIP(msg.toUtf8(), SkipSingle);
        }
    } else {
        QFAIL("returnHashError must be used after init has been executed.");
    }
}

void
DaemonTestCase::startUDMDaemon() {
    _daemonProcess = new QProcess();

    connect(_daemonProcess, static_cast<void(QProcess::*)
        (QProcess::ProcessError)>(&QProcess::error), this,
        &DaemonTestCase::onProcessError);
    QStringList args;
    args << "-daemon-path" << _daemonPath << "-disable-timeout"
        << "-stoppable";
    _daemonProcess->start(_daemonExec, args);

    // loop until the service is registered
    auto conn = QDBusConnection::sessionBus();
    while(!conn.interface()->isServiceRegistered(_daemonPath));
}

void
DaemonTestCase::stopUDMDaemon() {
    if (_daemonProcess != nullptr) {
        auto conn = QDBusConnection::sessionBus();
        returnDBusErrors(false);
        QDBusMessage exit = QDBusMessage::createMethodCall(_daemonPath,
            "/", "com.canonical.applications.DownloadManager", "exit");
        conn.send(exit);

        while(conn.interface()->isServiceRegistered(_daemonPath));
        _daemonProcess->waitForFinished();
    }
}

QString
DaemonTestCase::httpServerDir() {
    if (_httpServerDir.isEmpty()) {
        _httpServerDir = testDirectory() + "/http_server";
        qDebug() << "Server dir:" << _httpServerDir;
    }

    if (!QDir().exists(_httpServerDir))
        QDir().mkpath(_httpServerDir);

    return _httpServerDir;
}

void
DaemonTestCase::startHttpServer() {
    if (_httpServerRetry > 10) {
        QFAIL("Could not start http server.");
    }
    qDebug() << "Start http server";
    auto serverDir = httpServerDir();
    _httpServer = new QProcess();
    _httpServer->setWorkingDirectory(serverDir);
    QStringList args;
    args << "-m" << "http.server" << QString::number(_port);
    _httpServer->start("python3", args);
    _httpServer->waitForFinished(300);  // TODO: Find a better approach

    if (_httpServer->state() == QProcess::Running) {
        qDebug() << "Http server running on" << _port;
    } else {
        _httpServerRetry++;
        _port += 10;
        delete _httpServer;
        _httpServer = nullptr;
        startHttpServer();
    }
}

void
DaemonTestCase::stopHttpServer() {
    if (_httpServer != nullptr) {
        _httpServer->kill();
        _httpServer->waitForFinished();
        qDebug() << "Http server stopped" << _port;
    }
}

void
DaemonTestCase::onProcessError(QProcess::ProcessError error) {
    qDebug() << error << _daemonProcess->readAllStandardError()
        << _daemonProcess->readAllStandardOutput();
    QSKIP("Process could not be started.", SkipSingle);
}

void
DaemonTestCase::init() {
    BaseTestCase::init();
    // WARNING: create a path for this exact test.. we might have
    // issues if we have to two object with the same name
    _daemonPath = "com.canonical.applications.testing.Downloader."
        + objectName();

    startUDMDaemon();
    startHttpServer();
}

void
DaemonTestCase::cleanup() {
    stopHttpServer();
    delete _httpServer;

    stopUDMDaemon();
    delete _daemonProcess;

    BaseTestCase::cleanup();
}

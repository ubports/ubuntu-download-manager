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

#include "./test_download_daemon.h"

TestDownloadDaemon::TestDownloadDaemon(QObject *parent)
    : BaseTestCase(parent) {
}

void
TestDownloadDaemon::init() {
    BaseTestCase::init();
    _timer = new FakeTimer();
    _app = new FakeApplication();
    _appPointer = QSharedPointer<Application>(_app);
    _conn = new FakeDBusConnection();
    _man = new FakeDownloadManager(_appPointer,
        QSharedPointer<DBusConnection>(_conn));
    _daemon = new DownloadDaemon(_appPointer,
        _conn, _timer, _man, this);
}

void
TestDownloadDaemon::cleanup() {
    if (_app != NULL)
        delete _app;
    if (_conn != NULL)
        delete _conn;
    if (_daemon != NULL)
        delete _daemon;
    if (_timer != NULL)
        delete _timer;
    if (_man != NULL)
        delete _man;
}

void
TestDownloadDaemon::testStart() {
    _conn->setRegisterServiceResult(true);
    _conn->setRegisterObjectResult(true);
    _conn->record();
    _app->record();

    _daemon->start();

    QList<MethodData> calledMethods = _conn->calledMethods();

    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("registerService"), calledMethods[0].methodName());
    QCOMPARE(QString("registerObject"), calledMethods[1].methodName());

    // assert exit was NOT called
    calledMethods = _app->calledMethods();
    QCOMPARE(0, calledMethods.count());
}

void
TestDownloadDaemon::testStartFailServiceRegister() {
    _conn->setRegisterServiceResult(false);
    _conn->setRegisterObjectResult(true);
    _conn->record();
    _app->record();

    _daemon->start();

    QList<MethodData> calledMethods = _conn->calledMethods();

    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("registerService"), calledMethods[0].methodName());

    // assert exit was called
    calledMethods = _app->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("exit"), calledMethods[0].methodName());
}

void
TestDownloadDaemon::testStartFailObjectRegister() {
    _conn->setRegisterServiceResult(true);
    _conn->setRegisterObjectResult(false);
    _conn->record();
    _app->record();

    _daemon->start();

    QList<MethodData> calledMethods = _conn->calledMethods();

    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("registerService"), calledMethods[0].methodName());
    QCOMPARE(QString("registerObject"), calledMethods[1].methodName());

    // assert exit was called
    calledMethods = _app->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("exit"), calledMethods[0].methodName());
}

void
TestDownloadDaemon::testTimerStop() {
    _timer->setIsActive(true);
    _timer->record();
    _man->emitSizeChaged(1);

    QList<MethodData> calledMethods = _timer->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("isActive"), calledMethods[0].methodName());
    QCOMPARE(QString("stop"), calledMethods[1].methodName());
}

void
TestDownloadDaemon::testTimerStart() {
    _timer->setIsActive(false);
    _timer->record();
    _man->emitSizeChaged(0);

    QList<MethodData> calledMethods = _timer->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("isActive"), calledMethods[0].methodName());
    QCOMPARE(QString("start"), calledMethods[1].methodName());
}

void
TestDownloadDaemon::testTimeoutExit() {
    _app->record();
    // emit the timeout signal and assert that exit was called
    _timer->emitTimeout();

    QList<MethodData> calledMethods = _app->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("exit"), calledMethods[0].methodName());
}

void
TestDownloadDaemon::testDisableTimeout() {
    _timer->record();

    // set the args so that we disable the timeout
    QStringList args;
    args << "-disable-timeout";
    _app->setArguments(args);

    // assert that start is never called
    _daemon = new DownloadDaemon(_appPointer, _conn, _timer, _man, this);
    QList<MethodData> calledMethods = _timer->calledMethods();
    QCOMPARE(0, calledMethods.count());
}

void
TestDownloadDaemon::testSelfSignedCerts() {
    _man->record();
    QStringList args;
    args << "-self-signed-certs" << "*.pem";
    _app->setArguments(args);

    // assert that we set the certs
    _daemon = new DownloadDaemon(_appPointer, _conn, _timer, _man, this);
    QList<MethodData> calledMethods = _man->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("setAcceptedCertificates"), calledMethods[0].methodName());
}

void
TestDownloadDaemon::testSelfSignedCertsMissingPath() {
    _man->record();
    QStringList args;
    args << "-self-signed-certs";
    _app->setArguments(args);

    // assert that we do not crash
    _daemon = new DownloadDaemon(_appPointer, _conn, _timer, _man, this);
    QList<MethodData> calledMethods = _man->calledMethods();
    QCOMPARE(1, calledMethods.count());
}

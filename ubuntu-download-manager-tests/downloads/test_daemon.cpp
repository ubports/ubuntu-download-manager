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

#include "test_daemon.h"

TestDaemon::TestDaemon(QObject *parent)
    : BaseTestCase("TestDaemon", parent) {
}

void
TestDaemon::init() {
    BaseTestCase::init();
    _timer = new FakeTimer();
    _app = new FakeApplication();
    _appPointer = QSharedPointer<Application>(_app);
    _conn = new FakeDBusConnection();
    _man = new FakeDownloadManager(_appPointer,
        QSharedPointer<DBusConnection>(_conn));
    _daemon = new Daemon(_appPointer,
        _conn, _timer, _man, this);
}

void
TestDaemon::cleanup() {
    BaseTestCase::cleanup();

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
TestDaemon::testStart() {
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
TestDaemon::testStartFailServiceRegister() {
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
TestDaemon::testStartFailObjectRegister() {
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
TestDaemon::testTimerStop() {
    _timer->setIsActive(true);
    _timer->record();
    _man->emitSizeChaged(1);

    QList<MethodData> calledMethods = _timer->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("isActive"), calledMethods[0].methodName());
    QCOMPARE(QString("stop"), calledMethods[1].methodName());
}

void
TestDaemon::testTimerStart() {
    _timer->setIsActive(false);
    _timer->record();
    _man->emitSizeChaged(0);

    QList<MethodData> calledMethods = _timer->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("isActive"), calledMethods[0].methodName());
    QCOMPARE(QString("start"), calledMethods[1].methodName());
}

void
TestDaemon::testTimeoutExit() {
    _app->record();
    // emit the timeout signal and assert that exit was called
    _timer->emitTimeout();

    QList<MethodData> calledMethods = _app->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("exit"), calledMethods[0].methodName());
}

void
TestDaemon::testDisableTimeout() {
    _timer->record();

    // set the args so that we disable the timeout
    QStringList args;
    args << "-disable-timeout";
    _app->setArguments(args);

    // assert that start is never called
    _daemon = new Daemon(_appPointer, _conn, _timer, _man, this);
    QList<MethodData> calledMethods = _timer->calledMethods();
    QCOMPARE(0, calledMethods.count());
}

void
TestDaemon::testSelfSignedCerts() {
    _man->record();
    QStringList args;
    args << "-self-signed-certs" << "*.pem";
    _app->setArguments(args);

    // assert that we set the certs
    _daemon = new Daemon(_appPointer, _conn, _timer, _man, this);
    QList<MethodData> calledMethods = _man->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("setAcceptedCertificates"), calledMethods[0].methodName());
}

void
TestDaemon::testSelfSignedCertsMissingPath() {
    _man->record();
    QStringList args;
    args << "-self-signed-certs";
    _app->setArguments(args);

    // assert that we do not crash
    _daemon = new Daemon(_appPointer, _conn, _timer, _man, this);
    QList<MethodData> calledMethods = _man->calledMethods();
    QCOMPARE(1, calledMethods.count());
}
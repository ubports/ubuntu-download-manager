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
    : QObject(parent) {
}

void
TestDownloadDaemon::init() {
    _app = new FakeApplication();
    _conn = new FakeDBusConnection();
    _daemon = new DownloadDaemon(_app, _conn, this);
}

void
TestDownloadDaemon::cleanup() {
    if (_app != NULL)
        delete _app;
    if (_conn != NULL)
        delete _conn;
    if (_daemon != NULL)
        delete _daemon;
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

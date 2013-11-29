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

#include <QDebug>
#include "test_daemon.h"

TestDaemon::TestDaemon(QObject *parent)
    : BaseTestCase("TestDaemon", parent) {
}

void
TestDaemon::init() {
    BaseTestCase::init();
    _timer = new FakeTimer();
    _app = new FakeApplication();
    _conn = new FakeDBusConnection();
    _man = new FakeDownloadManager(_app, _conn);
    _daemon = new Daemon(_app, _conn, _timer, _man, this);
}

void
TestDaemon::cleanup() {
    BaseTestCase::cleanup();

    // the daemon destructor takes care of the rest
    delete _daemon;
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

    StringWrapper* wrapper = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().inParams()[0]);
    QCOMPARE(QString("com.canonical.applications.Downloader"),
        wrapper->value());

    QCOMPARE(QString("registerObject"), calledMethods[1].methodName());

    // assert exit was NOT called
    calledMethods = _app->calledMethods();
    QCOMPARE(0, calledMethods.count());
}

void
TestDaemon::testStartPath() {
    QString myPath = "com.canonical.tests";
    _conn->setRegisterServiceResult(true);
    _conn->setRegisterObjectResult(true);
    _conn->record();
    _app->record();

    _daemon->start(myPath);

    QList<MethodData> calledMethods = _conn->calledMethods();

    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("registerService"), calledMethods[0].methodName());

    StringWrapper* wrapper = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().inParams()[0]);
    QCOMPARE(myPath, wrapper->value());

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
    _daemon = new Daemon(_app, _conn, _timer, _man, this);
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
    _daemon = new Daemon(_app, _conn, _timer, _man, this);
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
    _daemon = new Daemon(_app, _conn, _timer, _man, this);
    QList<MethodData> calledMethods = _man->calledMethods();
    QCOMPARE(1, calledMethods.count());
}

void
TestDaemon::testStoppable_data() {
    QTest::addColumn<bool>("enabled");

    QTest::newRow("Enabled") << true;
    QTest::newRow("Disabled") << false;
}

void
TestDaemon::testStoppable() {
    QFETCH(bool, enabled);
    _daemon = new Daemon(_app, _conn, _timer, _man, this);
    _daemon->setStoppable(enabled);
    QCOMPARE(_daemon->isStoppable(), enabled);
}

void
TestDaemon::testSetTimeout_data() {
    QTest::addColumn<bool>("enabled");

    QTest::newRow("Enabled") << true;
    QTest::newRow("Disabled") << false;
}

void
TestDaemon::testSetTimeout() {
    QFETCH(bool, enabled);
    _daemon = new Daemon(_app, _conn, _timer, _man, this);
    _daemon->enableTimeout(enabled);
    QCOMPARE(enabled, _daemon->isTimeoutEnabled());
}

void
TestDaemon::testSetSelfSignedSslCerts() {
    QList<QSslCertificate> certs = QSslCertificate::fromPath(
        dataDirectory() + "/*.pem");
    _daemon = new Daemon(_app, _conn, _timer, _man, this);
    _daemon->setSelfSignedCerts(certs);
    QList<QSslCertificate> daemonCerts = _daemon->selfSignedCerts();
    QCOMPARE(certs.count(), daemonCerts.count());
    foreach(QSslCertificate cert, certs) {
        QVERIFY(daemonCerts.contains(cert));
    }
}

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

#include "test_daemon.h"

using namespace Ubuntu::DownloadManager;

TestDaemon::TestDaemon(QObject *parent)
    : BaseTestCase("TestDaemon", parent) {
}

void
TestDaemon::init() {
    BaseTestCase::init();
}

void
TestDaemon::cleanup() {
    BaseTestCase::cleanup();
    SystemNetworkInfo::deleteInstance();
    RequestFactory::deleteInstance();
}

void
TestDaemon::testStart() {
    auto timer = new FakeTimer();
    auto conn = new FakeDBusConnection();
    conn->setRegisterServiceResult(true);
    conn->setRegisterObjectResult(true);
    conn->record();
    auto app = new FakeApplication();
    app->record();
    auto man = new FakeDownloadManager(app, conn);
    auto factory = new FakeDownloadManagerFactory(man);

    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn, timer, this));
    daemon->start();

    QList<MethodData> calledMethods = conn->calledMethods();

    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("registerService"), calledMethods[0].methodName());

    StringWrapper* wrapper = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().inParams()[0]);
    QCOMPARE(QString("com.canonical.applications.Downloader"),
        wrapper->value());

    QCOMPARE(QString("registerObject"), calledMethods[1].methodName());

    // assert exit was NOT called
    calledMethods = app->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QVERIFY("exit" != calledMethods[0].methodName());
}

void
TestDaemon::testStartPath() {
    QString myPath = "com.canonical.tests";
    auto timer = new FakeTimer();
    auto conn = new FakeDBusConnection();
    conn->setRegisterServiceResult(true);
    conn->setRegisterObjectResult(true);
    conn->record();
    auto app = new FakeApplication();
    app->record();
    auto man = new FakeDownloadManager(app, conn);
    auto factory = new FakeDownloadManagerFactory(man);

    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn, timer, this));
    daemon->start(myPath);

    QList<MethodData> calledMethods = conn->calledMethods();

    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("registerService"), calledMethods[0].methodName());

    StringWrapper* wrapper = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().inParams()[0]);
    QCOMPARE(myPath, wrapper->value());

    QCOMPARE(QString("registerObject"), calledMethods[1].methodName());

    // assert exit was NOT called
    calledMethods = app->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QVERIFY("exit" != calledMethods[0].methodName());
}

void
TestDaemon::testStartFailServiceRegister() {
    auto timer = new FakeTimer();
    auto conn = new FakeDBusConnection();
    conn->setRegisterServiceResult(false);
    conn->setRegisterObjectResult(true);
    conn->record();
    auto app = new FakeApplication();
    app->record();
    auto man = new FakeDownloadManager(app, conn);
    auto factory = new FakeDownloadManagerFactory(man);

    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn, timer, this));
    daemon->start();

    QList<MethodData> calledMethods = conn->calledMethods();

    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("registerService"), calledMethods[0].methodName());

    // assert exit was called
    calledMethods = app->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("exit"), calledMethods[1].methodName());
}

void
TestDaemon::testStartFailObjectRegister() {
    auto timer = new FakeTimer();
    auto conn = new FakeDBusConnection();
    conn->setRegisterServiceResult(true);
    conn->setRegisterObjectResult(false);
    conn->record();
    auto app = new FakeApplication();
    app->record();
    auto man = new FakeDownloadManager(app, conn);
    auto factory = new FakeDownloadManagerFactory(man);

    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn, timer, this));
    daemon->start();

    QList<MethodData> calledMethods = conn->calledMethods();

    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("registerService"), calledMethods[0].methodName());
    QCOMPARE(QString("registerObject"), calledMethods[1].methodName());

    // assert exit was called
    calledMethods = app->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("exit"), calledMethods[1].methodName());
}

void
TestDaemon::testTimerStop() {
    auto timer = new FakeTimer();
    timer->setIsActive(true);
    timer->record();
    auto conn = new FakeDBusConnection();
    auto app = new FakeApplication();
    auto man = new FakeDownloadManager(app, conn);
    auto factory = new FakeDownloadManagerFactory(man);

    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn, timer, this));

    man->emitSizeChaged(1);

    QList<MethodData> calledMethods = timer->calledMethods();
    QCOMPARE(3, calledMethods.count());
    QCOMPARE(QString("isActive"), calledMethods[1].methodName());
    QCOMPARE(QString("stop"), calledMethods[2].methodName());
}

void
TestDaemon::testTimerStart() {
    auto timer = new FakeTimer();
    timer->setIsActive(false);
    timer->record();
    auto conn = new FakeDBusConnection();
    auto app = new FakeApplication();
    auto man = new FakeDownloadManager(app, conn);
    auto factory = new FakeDownloadManagerFactory(man);

    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn, timer, this));

    man->emitSizeChaged(0);

    QList<MethodData> calledMethods = timer->calledMethods();
    QCOMPARE(3, calledMethods.count());
    QCOMPARE(QString("isActive"), calledMethods[1].methodName());
    QCOMPARE(QString("start"), calledMethods[2].methodName());
}

void
TestDaemon::testTimeoutExit() {
    auto timer = new FakeTimer();
    auto conn = new FakeDBusConnection();
    auto app = new FakeApplication();
    auto man = new FakeDownloadManager(app, conn);
    auto factory = new FakeDownloadManagerFactory(man);

    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn, timer, this));
    app->record();

    // emit the timeout signal and assert that exit was called
    timer->emitTimeout();

    QList<MethodData> calledMethods = app->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("exit"), calledMethods[0].methodName());
}

void
TestDaemon::testDisableTimeout() {
    auto timer = new FakeTimer();
    timer->record();
    auto conn = new FakeDBusConnection();
    auto app = new FakeApplication();
    auto man = new FakeDownloadManager(app, conn);
    auto factory = new FakeDownloadManagerFactory(man);

    // set the args so that we disable the timeout
    QStringList args;
    args << "-disable-timeout";
    app->setArguments(args);

    // assert that start is never called
    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn, timer, this));
    QList<MethodData> calledMethods = timer->calledMethods();
    QCOMPARE(0, calledMethods.count());
}

void
TestDaemon::testSelfSignedCerts() {
    auto timer = new FakeTimer();
    auto conn = new FakeDBusConnection();
    auto app = new FakeApplication();
    auto man = new FakeDownloadManager(app, conn);
    auto factory = new FakeDownloadManagerFactory(man);
    man->record();

    QStringList args;
    args << "-self-signed-certs" << "*.pem";
    app->setArguments(args);

    // assert that we set the certs
    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn, timer, this));
    QList<MethodData> calledMethods = man->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("setAcceptedCertificates"), calledMethods[0].methodName());
}

void
TestDaemon::testSelfSignedCertsMissingPath() {
    auto timer = new FakeTimer();
    auto conn = new FakeDBusConnection();
    auto app = new FakeApplication();
    auto man = new FakeDownloadManager(app, conn);
    auto factory = new FakeDownloadManagerFactory(man);
    man->record();
    QStringList args;
    args << "-self-signed-certs";
    app->setArguments(args);

    // assert that we do not crash
    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn, timer, this));
    QList<MethodData> calledMethods = man->calledMethods();
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
    auto timer = new FakeTimer();
    auto conn = new FakeDBusConnection();
    auto app = new FakeApplication();
    auto man = new FakeDownloadManager(app, conn);
    auto factory = new FakeDownloadManagerFactory(man);

    QFETCH(bool, enabled);
    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn, timer, this));
    daemon->setStoppable(enabled);
    QCOMPARE(daemon->isStoppable(), enabled);
}

void
TestDaemon::testSetTimeout_data() {
    QTest::addColumn<bool>("enabled");

    QTest::newRow("Enabled") << true;
    QTest::newRow("Disabled") << false;
}

void
TestDaemon::testSetTimeout() {
    auto timer = new FakeTimer();
    auto conn = new FakeDBusConnection();
    auto app = new FakeApplication();
    auto man = new FakeDownloadManager(app, conn);
    auto factory = new FakeDownloadManagerFactory(man);

    QFETCH(bool, enabled);
    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn, timer, this));
    daemon->enableTimeout(enabled);
    QCOMPARE(enabled, daemon->isTimeoutEnabled());
}

void
TestDaemon::testSetSelfSignedSslCerts() {
    auto timer = new FakeTimer();
    auto conn = new FakeDBusConnection();
    auto app = new FakeApplication();
    auto man = new FakeDownloadManager(app, conn);
    auto factory = new FakeDownloadManagerFactory(man);

    QList<QSslCertificate> certs = QSslCertificate::fromPath(
        dataDirectory() + "/*.pem");
    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn, timer, this));
    daemon->setSelfSignedCerts(certs);
    QList<QSslCertificate> daemonCerts = daemon->selfSignedCerts();
    QCOMPARE(certs.count(), daemonCerts.count());
    foreach(QSslCertificate cert, certs) {
        QVERIFY(daemonCerts.contains(cert));
    }
}

QTEST_MAIN(TestDaemon)

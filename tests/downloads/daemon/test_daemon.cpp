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

#include <network_session.h>

#include "test_daemon.h"

using namespace Ubuntu::DownloadManager;
using ::testing::_;
using ::testing::Mock;
using ::testing::IsEmpty;
using ::testing::Return;
using ::testing::DefaultValue;

void
TestDaemon::init() {
    BaseTestCase::init();
    DefaultValue<QStringList>::Set(QStringList());
}

void
TestDaemon::cleanup() {
    DefaultValue<QStringList>::Clear();
    NetworkSession::deleteInstance();
    RequestFactory::deleteInstance();
    BaseTestCase::cleanup();
}

void
TestDaemon::testStart() {
    QStringList args;
    auto timer = new MockTimer();
    QScopedPointer<MockDBusConnection> conn(new MockDBusConnection());
    auto app = new MockApplication();
    auto man = new MockDownloadManager(app, conn.data());
    auto factory = new MockDownloadManagerFactory();

    // set mock expectations

    EXPECT_CALL(*app, arguments())
        .Times(1)
        .WillRepeatedly(Return(args));

    EXPECT_CALL(*conn.data(),
        registerService(QString("com.canonical.applications.Downloader")))
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*conn.data(), registerObject(_, _, _))
        .Times(1)
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*factory, createManager(_, _, _, _))
        .Times(1)
        .WillRepeatedly(Return(man));

    EXPECT_CALL(*man, setAcceptedCertificates(IsEmpty()))
        .Times(1);

    EXPECT_CALL(*timer, start(30000))
        .Times(1);

    EXPECT_CALL(*app, exit(_)).Times(0);

    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn.data(), timer, this));

    daemon->start();

    QVERIFY(Mock::VerifyAndClearExpectations(timer));
    QVERIFY(Mock::VerifyAndClearExpectations(conn.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(app));
    QVERIFY(Mock::VerifyAndClearExpectations(man));
    QVERIFY(Mock::VerifyAndClearExpectations(factory));
    QVERIFY(Mock::VerifyAndClearExpectations(timer));
}

void
TestDaemon::testStartPath() {
    QStringList args;
    QString myPath = "com.canonical.tests";
    auto timer = new MockTimer();
    QScopedPointer<MockDBusConnection> conn(new MockDBusConnection());
    auto app = new MockApplication();
    auto man = new MockDownloadManager(app, conn.data());
    auto factory = new MockDownloadManagerFactory();

    // set mock expectations

    EXPECT_CALL(*app, arguments())
        .Times(1)
        .WillRepeatedly(Return(args));

    EXPECT_CALL(*conn.data(),
        registerService(myPath))
            .Times(1)
            .WillRepeatedly(Return(true));
    EXPECT_CALL(*conn.data(), registerObject(_, _, _))
        .Times(1)
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*factory, createManager(app, conn.data(), _, _))
        .Times(1)
        .WillRepeatedly(Return(man));

    EXPECT_CALL(*man, setAcceptedCertificates(IsEmpty()))
        .Times(1);

    EXPECT_CALL(*timer, start(30000))
        .Times(1);

    EXPECT_CALL(*app, exit(_)).Times(0);

    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn.data(), timer, this));

    daemon->start(myPath);
    QVERIFY(Mock::VerifyAndClearExpectations(timer));
    QVERIFY(Mock::VerifyAndClearExpectations(conn.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(app));
    QVERIFY(Mock::VerifyAndClearExpectations(man));
    QVERIFY(Mock::VerifyAndClearExpectations(factory));
    QVERIFY(Mock::VerifyAndClearExpectations(timer));
}

void
TestDaemon::testStartFailServiceRegister() {
    QStringList args;
    auto timer = new MockTimer();
    QScopedPointer<MockDBusConnection> conn(new MockDBusConnection());
    auto app = new MockApplication();
    auto man = new MockDownloadManager(app, conn.data());
    auto factory = new MockDownloadManagerFactory();

    EXPECT_CALL(*app, arguments())
        .Times(1)
        .WillRepeatedly(Return(args));

    EXPECT_CALL(*conn.data(), registerObject(_, _, _))
        .Times(1)
        .WillRepeatedly(Return(false));

    EXPECT_CALL(*conn.data(),
        registerService(QString("com.canonical.applications.Downloader")))
            .Times(0);

    EXPECT_CALL(*factory, createManager(_, _, _, _))
        .Times(1)
        .WillRepeatedly(Return(man));

    EXPECT_CALL(*man, setAcceptedCertificates(IsEmpty()))
        .Times(1);

    EXPECT_CALL(*timer, start(30000))
        .Times(1);

    EXPECT_CALL(*app, exit(_)).Times(1);

    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn.data(), timer, this));

    daemon->start();

    QVERIFY(Mock::VerifyAndClearExpectations(timer));
    QVERIFY(Mock::VerifyAndClearExpectations(conn.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(app));
    QVERIFY(Mock::VerifyAndClearExpectations(man));
    QVERIFY(Mock::VerifyAndClearExpectations(factory));
    QVERIFY(Mock::VerifyAndClearExpectations(timer));
}

void
TestDaemon::testStartFailObjectRegister() {
    QStringList args;
    auto timer = new MockTimer();
    QScopedPointer<MockDBusConnection> conn(new MockDBusConnection());
    auto app = new MockApplication();
    auto man = new MockDownloadManager(app, conn.data());
    auto factory = new MockDownloadManagerFactory();

    EXPECT_CALL(*app, arguments())
        .Times(1)
        .WillRepeatedly(Return(args));

    EXPECT_CALL(*conn.data(), registerObject(_, _, _))
        .Times(1)
        .WillRepeatedly(Return(false));

    EXPECT_CALL(*conn.data(),
        registerService(QString("com.canonical.applications.Downloader")))
            .Times(0);
    
    EXPECT_CALL(*factory, createManager(app, conn.data(), _, _))
        .Times(1)
        .WillRepeatedly(Return(man));

    EXPECT_CALL(*timer, start(30000))
        .Times(1);

    EXPECT_CALL(*app, exit(_)).Times(1);

    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn.data(), timer, this));

    daemon->start();

    QVERIFY(Mock::VerifyAndClearExpectations(timer));
    QVERIFY(Mock::VerifyAndClearExpectations(conn.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(app));
    QVERIFY(Mock::VerifyAndClearExpectations(man));
    QVERIFY(Mock::VerifyAndClearExpectations(factory));
    QVERIFY(Mock::VerifyAndClearExpectations(timer));
}

void
TestDaemon::testTimerStop() {
    QStringList args;
    auto timer = new MockTimer();
    QScopedPointer<MockDBusConnection> conn(new MockDBusConnection());
    auto app = new MockApplication();
    auto man = new MockDownloadManager(app, conn.data());
    auto factory = new MockDownloadManagerFactory();

    EXPECT_CALL(*app, arguments())
        .Times(1)
        .WillRepeatedly(Return(args));

    EXPECT_CALL(*conn.data(),
        registerService(QString("com.canonical.applications.Downloader")))
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*conn.data(), registerObject(_, _, _))
        .Times(1)
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*factory, createManager(app, conn.data(), _, _))
        .Times(1)
        .WillRepeatedly(Return(man));

    EXPECT_CALL(*man, setAcceptedCertificates(IsEmpty()))
        .Times(1);

    EXPECT_CALL(*timer, start(30000))
        .Times(1);

    EXPECT_CALL(*timer, isActive())
        .Times(1)
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*timer, stop())
        .Times(1);

    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn.data(), timer, this));
    daemon->start();

    man->sizeChanged(1);

    QVERIFY(Mock::VerifyAndClearExpectations(timer));
    QVERIFY(Mock::VerifyAndClearExpectations(conn.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(app));
    QVERIFY(Mock::VerifyAndClearExpectations(man));
    QVERIFY(Mock::VerifyAndClearExpectations(factory));
    QVERIFY(Mock::VerifyAndClearExpectations(timer));
}


void
TestDaemon::testTimerStart() {
    QStringList args;
    auto timer = new MockTimer();
    QScopedPointer<MockDBusConnection> conn(new MockDBusConnection());
    auto app = new MockApplication();
    auto man = new MockDownloadManager(app, conn.data());
    auto factory = new MockDownloadManagerFactory();

    EXPECT_CALL(*app, arguments())
        .Times(1)
        .WillRepeatedly(Return(args));

    EXPECT_CALL(*conn.data(),
        registerService(QString("com.canonical.applications.Downloader")))
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*conn.data(), registerObject(_, _, _))
        .Times(1)
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*factory, createManager(app, conn.data(), _, _))
        .Times(1)
        .WillRepeatedly(Return(man));

    EXPECT_CALL(*man, setAcceptedCertificates(IsEmpty()))
        .Times(1);

    EXPECT_CALL(*timer, start(30000))
        .Times(2);  // most be started twice

    EXPECT_CALL(*timer, isActive())
        .Times(2)
        .WillOnce(Return(true))  // first attempt
        .WillOnce(Return(false)); // second attempt for the second signal

    EXPECT_CALL(*timer, stop())
        .Times(1);

    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn.data(), timer, this));
    daemon->start();

    man->sizeChanged(1);
    man->sizeChanged(0);

    QVERIFY(Mock::VerifyAndClearExpectations(timer));
    QVERIFY(Mock::VerifyAndClearExpectations(conn.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(app));
    QVERIFY(Mock::VerifyAndClearExpectations(man));
    QVERIFY(Mock::VerifyAndClearExpectations(factory));
    QVERIFY(Mock::VerifyAndClearExpectations(timer));
}

void
TestDaemon::testTimeoutExit() {
    QStringList args;
    auto timer = new MockTimer();
    QScopedPointer<MockDBusConnection> conn(new MockDBusConnection());
    auto app = new MockApplication();
    auto man = new MockDownloadManager(app, conn.data());
    auto factory = new MockDownloadManagerFactory();

    // set mock expectations

    EXPECT_CALL(*app, arguments())
        .Times(1)
        .WillRepeatedly(Return(args));

    EXPECT_CALL(*conn.data(),
        registerService(QString("com.canonical.applications.Downloader")))
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*conn.data(), registerObject(_, _, _))
        .Times(1)
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*factory, createManager(_, _, _, _))
        .Times(1)
        .WillRepeatedly(Return(man));

    EXPECT_CALL(*man, setAcceptedCertificates(IsEmpty()))
        .Times(1);

    EXPECT_CALL(*timer, start(30000))
        .Times(1);

    EXPECT_CALL(*app, exit(_)).Times(1);  // we must exit the app

    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn.data(), timer, this));

    daemon->start();
    timer->timeout();

    QVERIFY(Mock::VerifyAndClearExpectations(timer));
    QVERIFY(Mock::VerifyAndClearExpectations(conn.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(app));
    QVERIFY(Mock::VerifyAndClearExpectations(man));
    QVERIFY(Mock::VerifyAndClearExpectations(factory));
    QVERIFY(Mock::VerifyAndClearExpectations(timer));
}

void
TestDaemon::testDisableTimeout() {
    QStringList args;
    args << "-disable-timeout";

    auto timer = new MockTimer();
    QScopedPointer<MockDBusConnection> conn(new MockDBusConnection());
    auto app = new MockApplication();
    auto man = new MockDownloadManager(app, conn.data());
    auto factory = new MockDownloadManagerFactory();

    // set mock expectations

    EXPECT_CALL(*app, arguments())
        .Times(1)
        .WillRepeatedly(Return(args));

    EXPECT_CALL(*conn.data(),
        registerService(QString("com.canonical.applications.Downloader")))
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*conn.data(), registerObject(_, _, _))
        .Times(1)
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*factory, createManager(_, _, _, _))
        .Times(1)
        .WillRepeatedly(Return(man));

    EXPECT_CALL(*man, setAcceptedCertificates(IsEmpty()))
        .Times(1);

    EXPECT_CALL(*timer, start(30000))
        .Times(0);  // never start the timer

    EXPECT_CALL(*app, exit(_)).Times(0);

    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn.data(), timer, this));

    daemon->start();

    QVERIFY(Mock::VerifyAndClearExpectations(timer));
    QVERIFY(Mock::VerifyAndClearExpectations(conn.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(app));
    QVERIFY(Mock::VerifyAndClearExpectations(man));
    QVERIFY(Mock::VerifyAndClearExpectations(factory));
    QVERIFY(Mock::VerifyAndClearExpectations(timer));
}


void
TestDaemon::testSelfSignedCertsMissingPath() {
    QStringList args;
    args << "-self-signed-certs";
    auto timer = new MockTimer();
    QScopedPointer<MockDBusConnection> conn(new MockDBusConnection());
    auto app = new MockApplication();
    auto man = new MockDownloadManager(app, conn.data());
    auto factory = new MockDownloadManagerFactory();

    // set mock expectations

    EXPECT_CALL(*app, arguments())
        .Times(1)
        .WillRepeatedly(Return(args));

    EXPECT_CALL(*conn.data(),
        registerService(QString("com.canonical.applications.Downloader")))
            .Times(1)
            .WillRepeatedly(Return(true));

    EXPECT_CALL(*conn.data(), registerObject(_, _, _))
        .Times(1)
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*factory, createManager(_, _, _, _))
        .Times(1)
        .WillRepeatedly(Return(man));

    EXPECT_CALL(*man, setAcceptedCertificates(IsEmpty()))
        .Times(1);

    EXPECT_CALL(*timer, start(30000))
        .Times(1);

    EXPECT_CALL(*app, exit(_)).Times(0);

    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(factory, app, conn.data(), timer, this));

    daemon->start();

    QVERIFY(Mock::VerifyAndClearExpectations(timer));
    QVERIFY(Mock::VerifyAndClearExpectations(conn.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(app));
    QVERIFY(Mock::VerifyAndClearExpectations(man));
    QVERIFY(Mock::VerifyAndClearExpectations(factory));
    QVERIFY(Mock::VerifyAndClearExpectations(timer));
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
    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon(this));
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
    QFETCH(bool, enabled);
    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon());
    daemon->enableTimeout(enabled);
    QCOMPARE(enabled, daemon->isTimeoutEnabled());
}

void
TestDaemon::testSetSelfSignedSslCerts() {
    QList<QSslCertificate> certs = QSslCertificate::fromPath(
        dataDirectory() + "/*.pem");
    QScopedPointer<Daemon::DownloadDaemon> daemon(
        new Daemon::DownloadDaemon());
    daemon->setSelfSignedCerts(certs);
    QList<QSslCertificate> daemonCerts = daemon->selfSignedCerts();
    QCOMPARE(certs.count(), daemonCerts.count());
    foreach(QSslCertificate cert, certs) {
        QVERIFY(daemonCerts.contains(cert));
    }
}

QTEST_MAIN(TestDaemon)
#include "moc_test_daemon.cpp"

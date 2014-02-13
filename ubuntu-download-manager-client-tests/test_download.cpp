/*
 * Copyright 2014 Canonical Ltd.
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

#include <QNetworkReply>
#include <QSignalSpy>
#include <ubuntu/download_manager/error.h>
#include "test_download.h"

TestDownload::TestDownload(QObject *parent)
    : LocalTreeTestCase("TestDownload", parent) {
}

void
TestDownload::init() {
    LocalTreeTestCase::init();
    _man = Manager::createSessionManager(daemonPath(), this);

    _metadata["my-string"] = "canonical";
    _metadata["your-string"] = "developer";
    QMap<QString, QString> headers;
    _url = largeFileUrl().toString();
    DownloadStruct downStruct(_url, _metadata, headers);

    QSignalSpy managerSpy(_man, SIGNAL(downloadCreated(Download*)));
    _man->createDownload(downStruct);

    QTRY_COMPARE(1, managerSpy.count());
    _down = managerSpy.takeFirst().at(0).value<Download*>();
}

void
TestDownload::cleanup(){
    LocalTreeTestCase::cleanup();
    delete _down;
    delete _man;
}

void
TestDownload::testAllowMobileDownload_data() {
    QTest::addColumn<bool>("enabled");

    QTest::newRow("Enabled") << true;
    QTest::newRow("Disabled") << false;
}

void
TestDownload::testAllowMobileDownload() {
    QFETCH(bool, enabled);
    _down->allowMobileDownload(enabled);
    auto isEnabled = _down->isMobileDownloadAllowed();
    QCOMPARE(enabled, isEnabled);
}

void
TestDownload::testAllowMobileDownloadError() {
    returnDBusErrors(true);
    _down->allowMobileDownload(false);
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
    QCOMPARE(Error::DBus, _down->error()->type());
}

void
TestDownload::testSetThrottle_data() {
    QTest::addColumn<qulonglong>("speed");

    QTest::newRow("First row") << 200ULL;
    QTest::newRow("Second row") << 1212ULL;
    QTest::newRow("Third row") << 998ULL;
    QTest::newRow("Last row") << 60ULL;
}

void
TestDownload::testSetThrottle() {
    QFETCH(qulonglong, speed);
    _down->setThrottle(speed);
    auto currentSpeed = _down->throttle();
    QCOMPARE(speed, currentSpeed);
}

void
TestDownload::testSetThrottleError() {
    returnDBusErrors(true);
    _down->setThrottle(0);
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
    QCOMPARE(Error::DBus, _down->error()->type());
}

void
TestDownload::testMetadata() {
    auto currentMetadata = _down->metadata();
    foreach(auto key, _metadata.keys()) {
        QCOMPARE(currentMetadata[key], _metadata[key]);
    }
}

void
TestDownload::testMetadataError() {
    returnDBusErrors(true);
    _down->metadata();
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
    QCOMPARE(Error::DBus, _down->error()->type());
}

void
TestDownload::testProgressError() {
    returnDBusErrors(true);
    _down->progress();
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
    QCOMPARE(Error::DBus, _down->error()->type());
}

void
TestDownload::testTotalSizeError() {
    returnDBusErrors(true);
    _down->totalSize();
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
    QCOMPARE(Error::DBus, _down->error()->type());
}

void
TestDownload::testAuthErrorRaised_data() {
    QTest::addColumn<QNetworkReply::NetworkError>("code");
    QTest::addColumn<QString>("msg");

    QTest::newRow("Server Authentication")
        << QNetworkReply::AuthenticationRequiredError
        << "Server rejected request";
    QTest::newRow("Proxy Authentication")
        << QNetworkReply::ProxyAuthenticationRequiredError
        << "Proxy rejected request";
}

void
TestDownload::testAuthErrorRaised() {
    QFETCH(QNetworkReply::NetworkError, code);
    QFETCH(QString, msg);

    AuthErrorStruct err(code, msg);
    returnAuthError(_url, err);

    QSignalSpy spy(_down, SIGNAL(error(Error*)));
    _down->start();

    QTRY_COMPARE(1, spy.count());
    auto error = spy.takeFirst().at(0).value<Error*>();
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
    QCOMPARE(Error::Auth, error->type());

    auto authError = qobject_cast<AuthError*>(error);
    QCOMPARE(msg, authError->phrase());
}

void
TestDownload::testHttpErrorRaised_data() {
    QTest::addColumn<int>("code");
    QTest::addColumn<QString>("msg");

    QTest::newRow("400- Bad Request") << 400 << "Bad Request";
    QTest::newRow("401- Unauthorized") << 401 << "Unauthorized";
    QTest::newRow("402- Payment Required") << 402 << "Payment Required";
    QTest::newRow("403- Forbidden") << 403 << "Forbidden";
    QTest::newRow("404- Not Found") << 404 << "Not Found";
    QTest::newRow("406- Not Acceptable") << 406 << "Not Acceptable";
}

void
TestDownload::testHttpErrorRaised() {
    QFETCH(int, code);
    QFETCH(QString, msg);

    HttpErrorStruct err(code, msg);
    returnHttpError(_url, err);

    QSignalSpy spy(_down, SIGNAL(error(Error*)));
    _down->start();

    QTRY_COMPARE(1, spy.count());
    auto error = spy.takeFirst().at(0).value<Error*>();
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
    QCOMPARE(Error::Http, error->type());

    auto httpError = qobject_cast<HttpError*>(error);
    QCOMPARE(code, httpError->code());
    QCOMPARE(msg, httpError->phrase());
}

void
TestDownload::testNetworkErroRaised_data() {
    QTest::addColumn<int>("code");
    QTest::addColumn<QString>("msg");

    QTest::newRow("Connection Refused Error") << 1 << "Connection Refused Error"; 
    QTest::newRow("RemoteHost Closed Error") << 2 << "RemoteHost Closed Error"; 
    QTest::newRow("Host Not Found Error") << 3 << "Host Not Found Error"; 
    QTest::newRow("Timeout Error") << 4 << "Timeout Error"; 
    QTest::newRow("Operation Canceled Error") << 5 << "Operation Canceled Error"; 
}

void
TestDownload::testNetworkErroRaised() {
    QFETCH(int, code);
    QFETCH(QString, msg);

    NetworkErrorStruct err(code, msg);
    returnNetworkError(_url, err);

    QSignalSpy spy(_down, SIGNAL(error(Error*)));
    _down->start();

    QTRY_COMPARE(1, spy.count());
    auto error = spy.takeFirst().at(0).value<Error*>();
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
    QCOMPARE(Error::Network, error->type());

    auto networkError = qobject_cast<NetworkError*>(error);
    QCOMPARE(code, static_cast<int>(networkError->code()));
    QCOMPARE(msg, networkError->phrase());
}

void
TestDownload::testProcessErrorRaised_data() {
    QTest::addColumn<int>("code");
    QTest::addColumn<QString>("msg");
    QTest::addColumn<int>("exitCode");
    QTest::addColumn<QString>("standardOutput");
    QTest::addColumn<QString>("standardError");

    QTest::newRow("Failed To Start") << 0 << "Failed To Start"
        << -1 << "" << ""; 
    QTest::newRow("Crashed") << 1 << "Crashed" << -1 << "Calculating.."
        << "NULL pointer"; 
    QTest::newRow("Timedout") << 2 << "Timedout" << -1 << "stdout"
        << ""; 
}

void
TestDownload::testProcessErrorRaised() {
    QFETCH(int, code);
    QFETCH(QString, msg);
    QFETCH(int, exitCode);
    QFETCH(QString, standardOutput);
    QFETCH(QString, standardError);

    ProcessErrorStruct err(code, msg, exitCode, standardOutput,
        standardError);
    returnProcessError(_url, err);

    QSignalSpy spy(_down, SIGNAL(error(Error*)));
    _down->start();

    QTRY_COMPARE(1, spy.count());
    auto error = spy.takeFirst().at(0).value<Error*>();
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
    QCOMPARE(Error::Process, error->type());

    auto processError = qobject_cast<ProcessError*>(error);
    QCOMPARE(code, static_cast<int>(processError->code()));
    QCOMPARE(msg, processError->phrase());
    QCOMPARE(exitCode, processError->exitCode());
    QCOMPARE(standardOutput, processError->standardOut());
    QCOMPARE(standardError, processError->standardError());
}

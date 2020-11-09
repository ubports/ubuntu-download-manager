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

#include <QNetworkReply>
#include "test_network_error_transition.h"

using ::testing::Mock;

void
TestNetworkErrorTransition::init() {
    BaseTestCase::init();
    _down = new MockSMFileDownload();
    _s1 = new QState();
    _s2 = new QFinalState();
    _stateMachine = new QStateMachine();

    _stateMachine->addState(_s1);
    _stateMachine->addState(_s2);

    _transition = new NetworkErrorTransition(_down, _s1, _s2);
    _s1->addTransition(_transition);
    _stateMachine->setInitialState(_s1);
}

void
TestNetworkErrorTransition::cleanup() {
    BaseTestCase::cleanup();
    delete _transition;
    delete _down;
    delete _s1;
    delete _s2;
    delete _stateMachine;
}

void
TestNetworkErrorTransition::testOnTransition_data() {
    QTest::addColumn<QNetworkReply::NetworkError>("code");
    QTest::newRow("ConnectionRefusedError")
        << QNetworkReply::ConnectionRefusedError;
    QTest::newRow("RemoteHostClosedError")
        << QNetworkReply::RemoteHostClosedError;
    QTest::newRow("HostNotFoundError")
        << QNetworkReply::HostNotFoundError;
    QTest::newRow("TimeoutError")
        << QNetworkReply::TimeoutError;
    QTest::newRow("OperationCanceledError")
        << QNetworkReply::OperationCanceledError;
    QTest::newRow("SslHandshakeFailedError")
        << QNetworkReply::SslHandshakeFailedError;
    QTest::newRow("TemporaryNetworkFailureError")
        << QNetworkReply::TemporaryNetworkFailureError;
    QTest::newRow("NetworkSessionFailedError")
        << QNetworkReply::NetworkSessionFailedError;
    QTest::newRow("BackgroundRequestNotAllowedError")
        << QNetworkReply::BackgroundRequestNotAllowedError;
    QTest::newRow("ProxyConnectionRefusedError")
        << QNetworkReply::ProxyConnectionRefusedError;
    QTest::newRow("ProxyConnectionClosedError")
        << QNetworkReply::ProxyConnectionClosedError;
    QTest::newRow("ProxyNotFoundError")
        << QNetworkReply::ProxyNotFoundError;
    QTest::newRow("ProxyTimeoutError")
        << QNetworkReply::ProxyTimeoutError;
    QTest::newRow("ProxyAuthenticationRequiredError")
        << QNetworkReply::ProxyAuthenticationRequiredError;
    QTest::newRow("ContentAccessDenied")
        << QNetworkReply::ContentAccessDenied;
    QTest::newRow("ContentOperationNotPermittedError")
        << QNetworkReply::ContentOperationNotPermittedError;
    QTest::newRow("ContentNotFoundError")
        << QNetworkReply::ContentNotFoundError;
    QTest::newRow("AuthenticationRequiredError")
        << QNetworkReply::AuthenticationRequiredError;
    QTest::newRow("ContentReSendError")
        << QNetworkReply::ContentReSendError;
    QTest::newRow("ProtocolUnknownError")
        << QNetworkReply::ProtocolUnknownError;
    QTest::newRow("ProtocolInvalidOperationError")
        << QNetworkReply::ProtocolInvalidOperationError;
    QTest::newRow("UnknownNetworkError")
        << QNetworkReply::UnknownNetworkError;
    QTest::newRow("UnknownProxyError")
        << QNetworkReply::UnknownProxyError;
    QTest::newRow("UnknownContentError")
        << QNetworkReply::UnknownContentError;
    QTest::newRow("ProtocolFailure")
        << QNetworkReply::ProtocolFailure;
}

void
TestNetworkErrorTransition::testOnTransition() {
    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));
    SignalBarrier finishedSpy(_stateMachine, SIGNAL(finished()));
    QFETCH(QNetworkReply::NetworkError, code);

    EXPECT_CALL(*_down, emitNetworkError(code))
        .Times(1);

    _stateMachine->start();

    // ensure that we started
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    // raise the signal and assert that the correct method was called with the
    // correct error code
    _down->error(code);

    // ensure that we finished
    QVERIFY(finishedSpy.ensureSignalEmitted());
    QTRY_COMPARE(finishedSpy.count(), 1);

    QVERIFY(Mock::VerifyAndClearExpectations(_down));
}

QTEST_MAIN(TestNetworkErrorTransition)

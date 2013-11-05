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
#include <QNetworkReply>
#include <QSignalSpy>
#include <QSslError>
#include "test_file_download_sm.h"

TestFileDownloadSM::TestFileDownloadSM(QObject *parent)
    : BaseTestCase("TestFileDownloadSM", parent) {
}

void
TestFileDownloadSM::init() {
    BaseTestCase::init();
    _down = new FakeSMFileDownload();
    _stateMachine = new DownloadSM(_down, this);
}

void
TestFileDownloadSM::cleanup() {
    BaseTestCase::cleanup();
    delete _stateMachine;
    delete _down;
}

void
TestFileDownloadSM::moveToInit() {
    _down->raiseHeadRequestCompleted();
}

void
TestFileDownloadSM::moveToDownloading() {
}

void
TestFileDownloadSM::moveToDownloadingNotConnected() {
}

void
TestFileDownloadSM::moveToPaused() {
}

void
TestFileDownloadSM::moveToPausedNotConnected() {
}

void
TestFileDownloadSM::moveToDownloaded() {
}

void
TestFileDownloadSM::moveToHashing() {
}

void
TestFileDownloadSM::moveToPostProcessing() {
}

void
TestFileDownloadSM::testIdleError() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));
    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));
    QSignalSpy finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    _down->raiseNetworkError(QNetworkReply::ProtocolFailure);

    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("ERROR"));
}

void
TestFileDownloadSM::testIdleSslErrors() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));
    QList<QSslError> errors;
    errors.append(QSslError(QSslError::CertificateExpired));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));
    QSignalSpy finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    _down->raiseSslError(errors);

    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("ERROR"));
}

void
TestFileDownloadSM::testIdleHeadRequestCompleted() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseHeadRequestCompleted();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("INIT"));
}

void
TestFileDownloadSM::testInitError() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));
    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));
    QSignalSpy finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    // move to the state we are testing
    moveToInit();
    _down->raiseNetworkError(QNetworkReply::ProtocolFailure);

    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("ERROR"));
}

void
TestFileDownloadSM::testInitSslErrors() {
    QList<QSslError> errors;
    errors.append(QSslError(QSslError::CertificateExpired));

    QCOMPARE(_stateMachine->state(), QString("IDLE"));
    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));
    QSignalSpy finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    // move to the state we are testing
    moveToInit();
    _down->raiseSslError(errors);

    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("ERROR"));
}

void
TestFileDownloadSM::testInitDownloadingStarted() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToInit();
    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseDownloadingStarted();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("DOWNLOADING"));
}

void
TestFileDownloadSM::testDownloadingError() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testDownloadingSslErrors() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testDownloadingCanceled() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testDownloadingPaused() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testDownloadingConnectionLost() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testDownloadingNotConnectedConnectionEnabled() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testDownloadingNotConnectedPaused() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testDownloadingNotConnectedCanceled() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testPauseDownloadingStarted() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testPauseConnectionLost() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testPauseCanceled() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testPausedNotConnectedConnectionEnabled() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testPausedNotConnectedDownloadingStarted() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testPausedNotConnectedCanceled() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testDownloadedHashingStarted() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testDownloadedPostProcessingStarted() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testDownloadedFinished() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testDownloadedCanceled() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testHashingError() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testHashingFinished() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testPostProcessingError() {
    QFAIL("Not implemented");
}

void
TestFileDownloadSM::testPostProcessingFinished() {
    QFAIL("Not implemented");
}

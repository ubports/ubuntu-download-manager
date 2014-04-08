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
    delete _down;
    delete _stateMachine;

    BaseTestCase::cleanup();
}

void
TestFileDownloadSM::moveToInit() {
    _down->raiseHeadRequestCompleted();
}

void
TestFileDownloadSM::moveToDownloading() {
    _down->raiseHeadRequestCompleted();
    _down->raiseDownloadingStarted();
}

void
TestFileDownloadSM::moveToDownloadingNotConnected() {
    _down->raiseHeadRequestCompleted();
    _down->raiseDownloadingStarted();
    _down->raiseConnectionDisabled();
}

void
TestFileDownloadSM::moveToPaused() {
    _down->raiseHeadRequestCompleted();
    _down->raiseDownloadingStarted();
    _down->raisePaused();
}

void
TestFileDownloadSM::moveToPausedNotConnected() {
    _down->raiseHeadRequestCompleted();
    _down->raiseDownloadingStarted();
    _down->raisePaused();
    _down->raiseConnectionDisabled();
}

void
TestFileDownloadSM::moveToDownloaded() {
    _down->raiseHeadRequestCompleted();
    _down->raiseDownloadingStarted();
    _down->raiseCompleted();
}

void
TestFileDownloadSM::moveToHashing() {
    _down->raiseHeadRequestCompleted();
    _down->raiseDownloadingStarted();
    _down->raiseCompleted();
    _down->raiseHashingStarted();
}

void
TestFileDownloadSM::moveToPostProcessing() {
    _down->raiseHeadRequestCompleted();
    _down->raiseDownloadingStarted();
    _down->raiseCompleted();
    _down->raisePostProcessingStarted();
}

void
TestFileDownloadSM::testIdleError() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);
    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));
    SignalBarrier finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    _down->raiseNetworkError(QNetworkReply::ProtocolFailure);

    QVERIFY(finishedSpy.ensureSignalEmitted());
    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::ERROR);
}

void
TestFileDownloadSM::testIdleSslErrors() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);
    QList<QSslError> errors;
    errors.append(QSslError(QSslError::CertificateExpired));

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));
    SignalBarrier finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    _down->raiseSslError(errors);

    QVERIFY(finishedSpy.ensureSignalEmitted());
    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::ERROR);
}

void
TestFileDownloadSM::testIdleHeadRequestCompleted() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseHeadRequestCompleted();

    QVERIFY(changedSpy.ensureSignalEmitted());
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::INIT);
}

void
TestFileDownloadSM::testInitError() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);
    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));
    SignalBarrier finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    // move to the state we are testing
    moveToInit();
    _down->raiseNetworkError(QNetworkReply::ProtocolFailure);

    QVERIFY(finishedSpy.ensureSignalEmitted());
    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::ERROR);
}

void
TestFileDownloadSM::testInitSslErrors() {
    QList<QSslError> errors;
    errors.append(QSslError(QSslError::CertificateExpired));

    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);
    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));
    SignalBarrier finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    // move to the state we are testing
    moveToInit();
    _down->raiseSslError(errors);

    QVERIFY(finishedSpy.ensureSignalEmitted());
    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::ERROR);
}

void
TestFileDownloadSM::testInitDownloadingStarted() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToInit();
    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseDownloadingStarted();

    QVERIFY(changedSpy.ensureSignalEmitted());
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::DOWNLOADING);
}

void
TestFileDownloadSM::testDownloadingError() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);
    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));
    SignalBarrier finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    // move to the state we are testing
    moveToDownloading();
    _down->raiseNetworkError(QNetworkReply::ProtocolFailure);

    QVERIFY(finishedSpy.ensureSignalEmitted());
    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::ERROR);
}

void
TestFileDownloadSM::testDownloadingSslErrors() {
    QList<QSslError> errors;
    errors.append(QSslError(QSslError::CertificateExpired));

    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);
    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));
    SignalBarrier finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    // move to the state we are testing
    moveToDownloading();
    _down->raiseSslError(errors);

    QVERIFY(finishedSpy.ensureSignalEmitted());
    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::ERROR);
}

void
TestFileDownloadSM::testDownloadingCanceled() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloading();
    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseCanceled();

    QVERIFY(changedSpy.ensureSignalEmitted());
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::CANCELED);
}

void
TestFileDownloadSM::testDownloadingPaused() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloading();
    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raisePaused();

    QVERIFY(changedSpy.ensureSignalEmitted());
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::PAUSED);
}

void
TestFileDownloadSM::testDownloadingConnectionLost() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloading();
    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseConnectionDisabled();

    QVERIFY(changedSpy.ensureSignalEmitted());
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::DOWNLOADING_NOT_CONNECTED);
}

void
TestFileDownloadSM::testDownloadingDownloaded() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloading();
    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseCompleted();

    QVERIFY(changedSpy.ensureSignalEmitted());
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::DOWNLOADED);
}

void
TestFileDownloadSM::testDownloadingNotConnectedConnectionEnabled() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloadingNotConnected();
    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseConnectionEnabled();

    QVERIFY(changedSpy.ensureSignalEmitted());
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::DOWNLOADING);
}

void
TestFileDownloadSM::testDownloadingNotConnectedPaused() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloadingNotConnected();
    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raisePaused();

    QVERIFY(changedSpy.ensureSignalEmitted());
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::PAUSED_NOT_CONNECTED);
}

void
TestFileDownloadSM::testDownloadingNotConnectedCanceled() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloadingNotConnected();
    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseCanceled();

    QVERIFY(changedSpy.ensureSignalEmitted());
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::CANCELED);
}

void
TestFileDownloadSM::testPauseDownloadingStarted() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToPaused();
    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseDownloadingStarted();

    QVERIFY(changedSpy.ensureSignalEmitted());
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::DOWNLOADING);
}

void
TestFileDownloadSM::testPauseConnectionLost() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToPaused();
    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseConnectionDisabled();

    QTRY_COMPARE(changedSpy.count(), 1);
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::PAUSED_NOT_CONNECTED);
}

void
TestFileDownloadSM::testPauseCanceled() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToPaused();
    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseCanceled();

    QVERIFY(changedSpy.ensureSignalEmitted());
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::CANCELED);
}

void
TestFileDownloadSM::testPausedNotConnectedConnectionEnabled() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToPausedNotConnected();
    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseConnectionEnabled();

    QVERIFY(changedSpy.ensureSignalEmitted());
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::PAUSED);
}

void
TestFileDownloadSM::testPausedNotConnectedDownloadingStarted() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToPausedNotConnected();
    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseDownloadingStarted();

    QVERIFY(changedSpy.ensureSignalEmitted());
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::DOWNLOADING_NOT_CONNECTED);
}

void
TestFileDownloadSM::testPausedNotConnectedCanceled() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToPausedNotConnected();
    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseCanceled();

    QVERIFY(changedSpy.ensureSignalEmitted());
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::CANCELED);
}

void
TestFileDownloadSM::testDownloadedHashingStarted() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloaded();
    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseHashingStarted();

    QVERIFY(changedSpy.ensureSignalEmitted());
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::HASHING);
}

void
TestFileDownloadSM::testDownloadedPostProcessingStarted() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloaded();
    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raisePostProcessingStarted();

    QVERIFY(changedSpy.ensureSignalEmitted());
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::POST_PROCESSING);
}

void
TestFileDownloadSM::testDownloadedFinished() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);
    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));
    SignalBarrier finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloaded();
    _down->raiseFinished();

    QVERIFY(finishedSpy.ensureSignalEmitted());
    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::FINISHED);
}

void
TestFileDownloadSM::testDownloadedCanceled() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);
    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));
    SignalBarrier finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloaded();
    _down->raiseCanceled();

    QVERIFY(finishedSpy.ensureSignalEmitted());
    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::CANCELED);
}

void
TestFileDownloadSM::testHashingError() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToHashing();
    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseHashingError();

    QVERIFY(changedSpy.ensureSignalEmitted());
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::ERROR);
}

void
TestFileDownloadSM::testHashingFinished() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);
    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));
    SignalBarrier finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToHashing();
    _down->raiseFinished();

    QVERIFY(finishedSpy.ensureSignalEmitted());
    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::FINISHED);
}

void
TestFileDownloadSM::testHashingPostProcessing() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);

    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToHashing();
    SignalBarrier changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raisePostProcessingStarted();

    QVERIFY(changedSpy.ensureSignalEmitted());
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::POST_PROCESSING);
}

void
TestFileDownloadSM::testPostProcessingError() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);
    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));
    SignalBarrier finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToPostProcessing();
    _down->raisePostProcessingError();

    QVERIFY(finishedSpy.ensureSignalEmitted());
    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::ERROR);
}

void
TestFileDownloadSM::testPostProcessingFinished() {
    QCOMPARE(_stateMachine->state(), DownloadSM::IDLE);
    SignalBarrier startedSpy(_stateMachine, SIGNAL(started()));
    SignalBarrier finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToPostProcessing();
    _down->raiseFinished();

    QVERIFY(finishedSpy.ensureSignalEmitted());
    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), DownloadSM::FINISHED);
}

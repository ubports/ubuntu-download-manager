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
    QCOMPARE(_stateMachine->state(), QString("IDLE"));
    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));
    QSignalSpy finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    // move to the state we are testing
    moveToDownloading();
    _down->raiseNetworkError(QNetworkReply::ProtocolFailure);

    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("ERROR"));
}

void
TestFileDownloadSM::testDownloadingSslErrors() {
    QList<QSslError> errors;
    errors.append(QSslError(QSslError::CertificateExpired));

    QCOMPARE(_stateMachine->state(), QString("IDLE"));
    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));
    QSignalSpy finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    // move to the state we are testing
    moveToDownloading();
    _down->raiseSslError(errors);

    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("ERROR"));
}

void
TestFileDownloadSM::testDownloadingCanceled() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloading();
    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseCanceled();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("CANCELED"));
}

void
TestFileDownloadSM::testDownloadingPaused() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloading();
    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raisePaused();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("PAUSED"));
}

void
TestFileDownloadSM::testDownloadingConnectionLost() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloading();
    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseConnectionDisabled();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("DOWNLOADING_NOT_CONNECTED"));
}

void
TestFileDownloadSM::testDownloadingDownloaded() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloading();
    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseCompleted();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("DOWNLOADED"));
}

void
TestFileDownloadSM::testDownloadingNotConnectedConnectionEnabled() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloadingNotConnected();
    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseConnectionEnabled();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("DOWNLOADING"));
}

void
TestFileDownloadSM::testDownloadingNotConnectedPaused() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloadingNotConnected();
    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raisePaused();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("PAUSED_NOT_CONNECTED"));
}

void
TestFileDownloadSM::testDownloadingNotConnectedCanceled() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloadingNotConnected();
    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseCanceled();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("CANCELED"));
}

void
TestFileDownloadSM::testPauseDownloadingStarted() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToPaused();
    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseDownloadingStarted();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("DOWNLOADING"));
}

void
TestFileDownloadSM::testPauseConnectionLost() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToPaused();
    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseConnectionDisabled();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("PAUSED_NOT_CONNECTED"));
}

void
TestFileDownloadSM::testPauseCanceled() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToPaused();
    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseCanceled();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("CANCELED"));
}

void
TestFileDownloadSM::testPausedNotConnectedConnectionEnabled() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToPausedNotConnected();
    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseConnectionEnabled();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("PAUSED"));
}

void
TestFileDownloadSM::testPausedNotConnectedDownloadingStarted() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToPausedNotConnected();
    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseDownloadingStarted();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("DOWNLOADING_NOT_CONNECTED"));
}

void
TestFileDownloadSM::testPausedNotConnectedCanceled() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToPausedNotConnected();
    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseCanceled();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("CANCELED"));
}

void
TestFileDownloadSM::testDownloadedHashingStarted() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloaded();
    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseHashingStarted();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("HASHING"));
}

void
TestFileDownloadSM::testDownloadedPostProcessingStarted() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloaded();
    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raisePostProcessingStarted();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("POST_PROCESSING"));
}

void
TestFileDownloadSM::testDownloadedFinished() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));
    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));
    QSignalSpy finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloaded();
    _down->raiseFinished();

    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("FINISHED"));
}

void
TestFileDownloadSM::testDownloadedCanceled() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));
    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));
    QSignalSpy finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToDownloaded();
    _down->raiseCanceled();

    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("CANCELED"));
}

void
TestFileDownloadSM::testHashingError() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToHashing();
    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raiseHashingError();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("ERROR"));
}

void
TestFileDownloadSM::testHashingFinished() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));
    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));
    QSignalSpy finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToHashing();
    _down->raiseFinished();

    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("FINISHED"));
}

void
TestFileDownloadSM::testHashingPostProcessing() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));

    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToHashing();
    QSignalSpy changedSpy(_stateMachine, SIGNAL(stateChanged(QString)));
    _down->raisePostProcessingStarted();

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("POST_PROCESSING"));
}

void
TestFileDownloadSM::testPostProcessingError() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));
    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));
    QSignalSpy finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToPostProcessing();
    _down->raisePostProcessingError();

    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("ERROR"));
}

void
TestFileDownloadSM::testPostProcessingFinished() {
    QCOMPARE(_stateMachine->state(), QString("IDLE"));
    QSignalSpy startedSpy(_stateMachine, SIGNAL(started()));
    QSignalSpy finishedSpy(_stateMachine, SIGNAL(finished()));

    _stateMachine->start();
    QTRY_COMPARE(startedSpy.count(), 1);

    moveToPostProcessing();
    _down->raiseFinished();

    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(_stateMachine->state(), QString("FINISHED"));
}

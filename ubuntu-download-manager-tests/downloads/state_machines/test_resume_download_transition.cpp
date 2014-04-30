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

#include "test_resume_download_transition.h"

TestResumeDownloadTransition::TestResumeDownloadTransition(QObject *parent)
    : BaseTestCase("TestResumeDownloadTransition", parent) {
}

void
TestResumeDownloadTransition::init() {
    BaseTestCase::init();
    _down = new FakeSMFileDownload();
    _s1 = new QState();
    _s2 = new QFinalState();

    _stateMachine.addState(_s1);
    _stateMachine.addState(_s2);

    _transition = new ResumeDownloadTransition(_down,
        SIGNAL(downloadingStarted()), _s1, _s2);
    _s1->addTransition(_transition);
    _stateMachine.setInitialState(_s1);
}

void
TestResumeDownloadTransition::cleanup() {
    BaseTestCase::cleanup();
    _stateMachine.removeState(_s1);
    _stateMachine.removeState(_s2);
    delete _transition;
    delete _down;
    delete _s1;
    delete _s2;
}

void
TestResumeDownloadTransition::testOnTransition() {
    _down->record();

    SignalBarrier startedSpy(&_stateMachine, SIGNAL(started()));
    SignalBarrier finishedSpy(&_stateMachine, SIGNAL(finished()));

    _stateMachine.start();
    // ensure that we started
    QVERIFY(startedSpy.ensureSignalEmitted());
    QTRY_COMPARE(startedSpy.count(), 1);

    // raise the signal and assert that the correct method was called with the
    // correct error code
    _down->raiseDownloadingStarted();

    // ensure that we finished
    QVERIFY(finishedSpy.ensureSignalEmitted());
    QTRY_COMPARE(finishedSpy.count(), 1);

    QList<MethodData> calledMethods = _down->calledMethods();
    QCOMPARE(calledMethods.count(), 1);
    QString methodName = calledMethods[0].methodName();
    QCOMPARE(methodName, QString("requestDownload"));
    QCOMPARE(Download::RESUME, _down->state());
}

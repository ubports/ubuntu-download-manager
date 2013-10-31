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

#include <QSignalSpy>
#include "test_start_download_transition.h"

TestStartDownloadTransition::TestStartDownloadTransition(QObject *parent)
    : BaseTestCase("TestStartDownloadTransition", parent) {
}

void
TestStartDownloadTransition::init() {
    BaseTestCase::init();
    _down = new FakeSMFileDownload();
    _s1 = new QState();
    _s2 = new QFinalState();

    _stateMachine.addState(_s1);
    _stateMachine.addState(_s2);

    _transition = new StartDownloadTransition(_down, _s1, _s2);
    _s1->addTransition(_transition);
    _stateMachine.setInitialState(_s1);
}

void
TestStartDownloadTransition::cleanup() {
    BaseTestCase::cleanup();
    _stateMachine.removeState(_s1);
    _stateMachine.removeState(_s2);
    if (_transition != NULL)
        delete _transition;
    if (_down != NULL)
        delete _down;
    if (_s1 != NULL)
        delete _s1;
    if (_s2 != NULL)
        delete _s2;
}

void
TestStartDownloadTransition::testOnTransition() {
    _down->record();

    QSignalSpy startedSpy(&_stateMachine, SIGNAL(started()));
    QSignalSpy finishedSpy(&_stateMachine, SIGNAL(finished()));

    _stateMachine.start();
    // ensure that we started
    QTRY_COMPARE(startedSpy.count(), 1);

    // raise the signal and assert that the correct method was called with the
    // correct error code
    _down->raiseDownloadingStarted();

    // ensure that we finished
    QTRY_COMPARE(finishedSpy.count(), 1);

    QList<MethodData> calledMethods = _down->calledMethods();
    QCOMPARE(calledMethods.count(), 1);
    QString methodName = calledMethods[0].methodName();
    QCOMPARE(methodName, QString("requestDownload"));
    QCOMPARE(Download::START, _down->state());
}

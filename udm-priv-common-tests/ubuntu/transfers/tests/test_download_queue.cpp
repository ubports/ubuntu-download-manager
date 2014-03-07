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

#include <QSignalSpy>
#include <ubuntu/transfers/system/uuid_utils.h>
#include "test_download_queue.h"

namespace Ubuntu {

namespace Transfers {

namespace Tests {

TestTransferQueue::TestTransferQueue(QObject *parent)
    : BaseTestCase("TestTransferQueue", parent) {
}

void
TestTransferQueue::init() {
    BaseTestCase::init();
    _isConfined = true;
    _rootPath = "/random/root/path";
    _networkInfo = new FakeSystemNetworkInfo();
    SystemNetworkInfo::setInstance(_networkInfo);
    _first = new FakeTransfer(UuidUtils::getDBusString(QUuid::createUuid()),
        "first-path", _isConfined);
    _second = new FakeTransfer(UuidUtils::getDBusString(QUuid::createUuid()),
        "second-path", _isConfined);
    _q = new Queue();
}

void
TestTransferQueue::cleanup() {
    BaseTestCase::cleanup();

    SystemNetworkInfo::deleteInstance();
    delete _first;
    delete _second;
    delete _q;
}

void
TestTransferQueue::testAddTransfer() {
    // test that when a transfer added the add signals is raised
    QSignalSpy spy(_q, SIGNAL(transferAdded(QString)));
    _q->add(_first);

    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());
}

void
TestTransferQueue::testStartTransferWithNoCurrent() {
    // add a transfer, set the state to start and assert that it will be started
    _first->record();
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->start();  // emit signal
    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());

    // state that startTransfer was called in first
    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
    QCOMPARE(QString("startTransfer"), calledMethods[1].methodName());
}

void
TestTransferQueue::testStartTransferWithCurrent() {
    // add a transfer, start it, add a second one and
    // assert that we did not start it right away
    _first->record();
    _second->record();

    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _first->start();  // emit signal
    _q->add(_second);
    _second->start();

    QCOMPARE(spy.count(), 1);  // just raised by the first change of state

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());

    // state that startTransfer was called in first
    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
    QCOMPARE(QString("startTransfer"), calledMethods[1].methodName());

    // state that startTransfer was NOT called in second
    calledMethods = _second->calledMethods();
    QCOMPARE(0, calledMethods.count());
}

void
TestTransferQueue::testStartTransferWithNoCurrentCannotTransfer() {
    // tell the fake that it cannot transfer (GSM enabled)
    _first->setCanTransfer(false);
    _first->record();
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->start();  // emit signal
    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), QString(""));

    // state that startTransfer was called in first
    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(1, calledMethods.count());  // just canTransfer should be called
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
}

void
TestTransferQueue::testPauseTransferNoOtherReady() {
    // add, start and pause a transfer, this should,
    // chage current to "" and emit the required
    // signals and execute the required methods
    _first->record();
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);

    QVERIFY(_q->currentTransfer().isEmpty());

    _first->start();
    _first->pause();
    QVERIFY(_q->currentTransfer().isEmpty());

    QCOMPARE(spy.count(), 2);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());
    arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString().isEmpty());

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(5, calledMethods.count());
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
    QCOMPARE(QString("startTransfer"), calledMethods[1].methodName());
    QCOMPARE(QString("pauseTransfer"), calledMethods[2].methodName());
    QCOMPARE(QString("canTransfer"), calledMethods[3].methodName());
}

void
TestTransferQueue::testPauseTransferOtherReady() {
    // add two transfers, start them, pause the first
    // and assert that the second one is started
    _first->record();
    _second->record();

    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->start();
    _second->start();
    _first->pause();
    QCOMPARE(_q->currentTransfer(), _second->path());

    QCOMPARE(spy.count(), 2);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _second->path());

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(5, calledMethods.count());
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
    QCOMPARE(QString("startTransfer"), calledMethods[1].methodName());
    QCOMPARE(QString("pauseTransfer"), calledMethods[2].methodName());

    calledMethods = _second->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
    QCOMPARE(QString("startTransfer"), calledMethods[1].methodName());
}

void
TestTransferQueue::testResumeTransferNoOtherPresent() {
    // add, start, pause, resume and assert that the same guy
    // is used even when other paused transfers are present
    _first->record();
    _second->record();

    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->start();
    _first->pause();
    _first->resume();

    QCOMPARE(_q->currentTransfer(), _first->path());

    QCOMPARE(spy.count(), 3);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());
    arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString().isEmpty());
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(7, calledMethods.count());
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
    QCOMPARE(QString("startTransfer"), calledMethods[1].methodName());
    QCOMPARE(QString("pauseTransfer"), calledMethods[2].methodName());
    QCOMPARE(QString("canTransfer"), calledMethods[3].methodName());
    QCOMPARE(QString("canTransfer"), calledMethods[4].methodName());
    QCOMPARE(QString("canTransfer"), calledMethods[5].methodName());
    QCOMPARE(QString("resumeTransfer"), calledMethods[6].methodName());

    calledMethods = _second->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
}

void
TestTransferQueue::testResumeTransferOtherPresent() {
    // add tow, start, pause, resume and assert that
    // resume does not change the current one
    _first->record();
    _second->record();

    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->start();
    _first->pause();
    _second->start();
    _first->resume();

    QCOMPARE(_q->currentTransfer(), _second->path());

    QCOMPARE(spy.count(), 3);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());
    arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString().isEmpty());
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _second->path());

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(6, calledMethods.count());
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
    QCOMPARE(QString("startTransfer"), calledMethods[1].methodName());
    QCOMPARE(QString("pauseTransfer"), calledMethods[2].methodName());
    QCOMPARE(QString("canTransfer"), calledMethods[3].methodName());
    QCOMPARE(QString("canTransfer"), calledMethods[4].methodName());

    calledMethods = _second->calledMethods();
    QCOMPARE(3, calledMethods.count());
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
    QCOMPARE(QString("canTransfer"), calledMethods[1].methodName());
    QCOMPARE(QString("startTransfer"), calledMethods[2].methodName());
}

void
TestTransferQueue::testResumeTransferNoOtherPresentCannotTransfer() {
    // add, start, pause, resume and assert that the same guy
    // is used even when other paused transfers are present
    _first->record();
    _second->record();

    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->start();
    _first->pause();
    _first->setCanTransfer(false);
    _first->resume();

    QCOMPARE(_q->currentTransfer(), QString(""));

    QCOMPARE(spy.count(), 3);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());
    arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString().isEmpty());
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), QString(""));

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(6, calledMethods.count());  // only canTransfer start and pause
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
    QCOMPARE(QString("startTransfer"), calledMethods[1].methodName());
    QCOMPARE(QString("pauseTransfer"), calledMethods[2].methodName());
    QCOMPARE(QString("canTransfer"), calledMethods[3].methodName());

    calledMethods = _second->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
    QCOMPARE(QString("canTransfer"), calledMethods[1].methodName());
}

void
TestTransferQueue::testCancelTransferNoOtherReady() {
    // cancel the transfer and expect it to be done
    _first->record();
    QSignalSpy changedSpy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);

    QVERIFY(_q->currentTransfer().isEmpty());

    _first->start();
    _first->cancel();
    QVERIFY(_q->currentTransfer().isEmpty());

    QCOMPARE(changedSpy.count(), 2);

    QList<QVariant> arguments = changedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());
    arguments = changedSpy.takeFirst();
    QVERIFY(arguments.at(0).toString().isEmpty());

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(3, calledMethods.count());
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
    QCOMPARE(QString("startTransfer"), calledMethods[1].methodName());
    QCOMPARE(QString("cancelTransfer"), calledMethods[2].methodName());
}

void
TestTransferQueue::testCancelTransferOtherReady() {
    // start and cancel and assert the second one is started
    _first->record();
    _second->record();

    QSignalSpy changedSpy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->start();
    _second->start();
    _first->cancel();
    QCOMPARE(_q->currentTransfer(), _second->path());

    QCOMPARE(changedSpy.count(), 2);

    QList<QVariant> arguments = changedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());
    arguments = changedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _second->path());

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(3, calledMethods.count());
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
    QCOMPARE(QString("startTransfer"), calledMethods[1].methodName());
    QCOMPARE(QString("cancelTransfer"), calledMethods[2].methodName());

    calledMethods = _second->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
    QCOMPARE(QString("startTransfer"), calledMethods[1].methodName());
}

void
TestTransferQueue::testCancelTransferOtherReadyCannotTransfer() {
    // start and cancel and assert the second one is not started
    _first->record();
    _second->setCanTransfer(false);
    _second->record();

    QSignalSpy changedSpy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->start();
    _second->start();
    _first->cancel();
    QCOMPARE(_q->currentTransfer(), QString(""));

    QCOMPARE(changedSpy.count(), 2);

    QList<QVariant> arguments = changedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());
    arguments = changedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), QString(""));

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(3, calledMethods.count());
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
    QCOMPARE(QString("startTransfer"), calledMethods[1].methodName());
    QCOMPARE(QString("cancelTransfer"), calledMethods[2].methodName());

    calledMethods = _second->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
}

void
TestTransferQueue::testCancelTransferNotStarted() {
    // cancel not started and ensure that it is removed
    _first->record();
    QSignalSpy removedSpy(_q, SIGNAL(transferRemoved(QString)));
    _q->add(_first);

    QVERIFY(_q->currentTransfer().isEmpty());

    _first->cancel();
    QVERIFY(_q->currentTransfer().isEmpty());
}

void
TestTransferQueue::testTransfers() {
    // add the transfers to the q and assert that they are all returned
    _q->add(_first);
    _q->add(_second);

    QHash<QString, Transfer*> transfers = _q->transfers();
    QCOMPARE(transfers[_first->path()], _first);
    QCOMPARE(transfers[_second->path()], _second);
}

void
TestTransferQueue::testTransferFinishedOtherReady() {
    _first->record();
    _second->record();

    _q->add(_first);
    _q->add(_second);

    _first->start();
    _second->start();
    _first->setState(Transfer::FINISH);

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
    QCOMPARE(QString("startTransfer"), calledMethods[1].methodName());

    calledMethods = _second->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
    QCOMPARE(QString("startTransfer"), calledMethods[1].methodName());
}

void
TestTransferQueue::testTransferErrorWithOtherReady() {
    _first->record();
    _second->record();

    _q->add(_first);
    _q->add(_second);

    _first->start();
    _second->start();
    _first->setState(Transfer::ERROR);

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
    QCOMPARE(QString("startTransfer"), calledMethods[1].methodName());

    calledMethods = _second->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("canTransfer"), calledMethods[0].methodName());
    QCOMPARE(QString("startTransfer"), calledMethods[1].methodName());
}

void
TestTransferQueue::testNewUnmanagedIncreasesNumber() {
    _q->add(_first);

    QCOMPARE(1, _q->size());
    _second->setAddToQueue(false);

    _q->add(_second);
    QCOMPARE(2, _q->size());
}

void
TestTransferQueue::testErrorUnmanagedDecreasesNumber() {
    QSignalSpy spy(_q, SIGNAL(transferRemoved(QString)));
    _first->setAddToQueue(false);

    _q->add(_first);

    QCOMPARE(1, _q->size());
    _first->setState(Transfer::ERROR);

    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(0, _q->size());
}

void
TestTransferQueue::testFinishUnmanagedDecreasesNumber() {
    QSignalSpy spy(_q, SIGNAL(transferRemoved(QString)));
    _first->setAddToQueue(false);

    _q->add(_first);

    QCOMPARE(1, _q->size());
    _first->setState(Transfer::FINISH);

    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(0, _q->size());
}

void
TestTransferQueue::testCancelUnmanagedDecreasesNumber() {
    QSignalSpy spy(_q, SIGNAL(transferRemoved(QString)));
    _first->setAddToQueue(false);

    _q->add(_first);

    QCOMPARE(1, _q->size());
    _first->cancel();

    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(0, _q->size());
}

}  // Tests

}  // Transfers

}  // Ubuntu

QTEST_MAIN(TestDownloadQueue)

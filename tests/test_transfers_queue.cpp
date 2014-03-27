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
#include "test_transfers_queue.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::AnyNumber;
using ::testing::Return;
using ::testing::Return;
using ::testing::AnyOf;

void
TestTransferQueue::verifyMocks() {
    QVERIFY(Mock::VerifyAndClearExpectations(_networkInfo));
    QVERIFY(Mock::VerifyAndClearExpectations(_first));
    QVERIFY(Mock::VerifyAndClearExpectations(_second));
}

void
TestTransferQueue::init() {
    BaseTestCase::init();
    _isConfined = true;
    _rootPath = "/random/root/path";
    _networkInfo = new MockSystemNetworkInfo();
    SystemNetworkInfo::setInstance(_networkInfo);
    _first = new MockTransfer(UuidUtils::getDBusString(QUuid::createUuid()),
        "first-path", _isConfined);
    _second = new MockTransfer(UuidUtils::getDBusString(QUuid::createUuid()),
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
    EXPECT_CALL(*_first, addToQueue())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_first, path())
        .Times(1)
        .WillRepeatedly(Return(QString("path")));

    // test that when a transfer added the add signals is raised
    QSignalSpy spy(_q, SIGNAL(transferAdded(QString)));
    _q->add(_first);

    QCOMPARE(spy.count(), 1);
    verifyMocks();
}

void
TestTransferQueue::testStartTransferWithNoCurrent() {
    auto path = QString("path");
    EXPECT_CALL(*_first, addToQueue())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_first, state())
        .Times(2)
        .WillRepeatedly(Return(Transfer::START));

    EXPECT_CALL(*_first, path())
        .Times(1)
        .WillRepeatedly(Return(path));

    EXPECT_CALL(*_first, canTransfer())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_first, startTransfer())
        .Times(1);

    // add a transfer, set the state to start and assert that it will
    // be started
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->stateChanged();
    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), path);
    verifyMocks();
}

void
TestTransferQueue::testStartTransferWithCurrent() {
    auto path = QString("path");
    auto secondPath = QString("second path");
    EXPECT_CALL(*_first, addToQueue())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_first, state())
        .Times(AnyNumber())
        .WillRepeatedly(Return(Transfer::START));

    EXPECT_CALL(*_first, path())
        .Times(1)
        .WillRepeatedly(Return(path));

    EXPECT_CALL(*_first, canTransfer())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_first, startTransfer())
        .Times(1);

    // second transfer expectations
    EXPECT_CALL(*_second, addToQueue())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_second, state())
        .Times(AnyNumber())
        .WillRepeatedly(Return(Transfer::START));

    EXPECT_CALL(*_second, path())
        .Times(1)
        .WillRepeatedly(Return(secondPath));

    EXPECT_CALL(*_second, canTransfer())
        .Times(0);

    EXPECT_CALL(*_second, startTransfer())
        .Times(0);

    // add a transfer, set the state to start and assert that it will
    // be started
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->stateChanged();
    _first->stateChanged();
    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), path);
    verifyMocks();
}

void
TestTransferQueue::testStartTransferWithNoCurrentCannotTransfer() {
    auto path = QString("path");
    EXPECT_CALL(*_first, addToQueue())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_first, state())
        .Times(2)
        .WillRepeatedly(Return(Transfer::START));

    EXPECT_CALL(*_first, path())
        .Times(1)
        .WillRepeatedly(Return(path));

    EXPECT_CALL(*_first, canTransfer())
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*_first, startTransfer())
        .Times(0);

    // add a transfer, set the state to start and assert that it will
    // be started
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->stateChanged();
    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), QString(""));
    verifyMocks();
}

void
TestTransferQueue::testPauseTransferNoOtherReady() {
    auto path = QString("path");
    EXPECT_CALL(*_first, addToQueue())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_first, state())
        .Times(5)
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::PAUSE))
        .WillOnce(Return(Transfer::PAUSE))
        .WillOnce(Return(Transfer::PAUSE));

    EXPECT_CALL(*_first, path())
        .Times(AnyNumber())
        .WillRepeatedly(Return(path));

    EXPECT_CALL(*_first, canTransfer())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_first, startTransfer())
        .Times(1);

    EXPECT_CALL(*_first, pauseTransfer())
        .Times(1);

    // add a transfer, set the state to start and assert that it will
    // be started
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->stateChanged();
    _first->stateChanged();
    QCOMPARE(spy.count(), 2);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), path);
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), QString(""));
    verifyMocks();
}

void
TestTransferQueue::testPauseTransferOtherReady() {
    auto path = QString("path");
    EXPECT_CALL(*_first, addToQueue())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_first, state())
        .Times(AnyNumber())
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::PAUSE))
        .WillOnce(Return(Transfer::PAUSE))
        .WillOnce(Return(Transfer::PAUSE))
        .WillOnce(Return(Transfer::PAUSE))
        .WillOnce(Return(Transfer::PAUSE));

    EXPECT_CALL(*_first, path())
        .Times(AnyNumber())
        .WillRepeatedly(Return(path));

    EXPECT_CALL(*_first, canTransfer())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_first, startTransfer())
        .Times(1);

    EXPECT_CALL(*_first, pauseTransfer())
        .Times(1);

    // second transfer expectations
    auto secondPath = QString("second path");
    EXPECT_CALL(*_second, addToQueue())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_second, state())
        .Times(AnyNumber())
        .WillRepeatedly(Return(Transfer::START));

    EXPECT_CALL(*_second, path())
        .Times(1)
        .WillRepeatedly(Return(secondPath));

    EXPECT_CALL(*_second, canTransfer())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_second, startTransfer())
        .Times(1);

    // add a transfer, set the state to start and assert that it will
    // be started
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->stateChanged();
    _second->stateChanged();
    _first->stateChanged();
    QCOMPARE(spy.count(), 2);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), path);
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), secondPath);
    verifyMocks();
}

void
TestTransferQueue::testResumeTransferNoOtherPresent() {
    auto path = QString("path");
    EXPECT_CALL(*_first, addToQueue())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_first, state())
        .Times(AnyNumber())
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::PAUSE))
        .WillOnce(Return(Transfer::PAUSE))
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::START));

    EXPECT_CALL(*_first, path())
        .Times(AnyNumber())
        .WillRepeatedly(Return(path));

    EXPECT_CALL(*_first, canTransfer())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_first, startTransfer())
        .Times(2);  // assert that we are called twice

    EXPECT_CALL(*_first, pauseTransfer())
        .Times(1);

    // second transfer expectations
    auto secondPath = QString("second path");
    EXPECT_CALL(*_second, addToQueue())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_second, state())
        .Times(AnyNumber())
        .WillRepeatedly(Return(Transfer::PAUSE));

    EXPECT_CALL(*_second, path())
        .Times(1)
        .WillRepeatedly(Return(secondPath));

    EXPECT_CALL(*_second, startTransfer())
        .Times(0);  // assert that we never try to start a paused download

    // add a transfer, set the state to start and assert that it will
    // be started
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->stateChanged();
    _first->stateChanged();
    QCOMPARE(spy.count(), 2);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), path);
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), path);
    verifyMocks();
}

void
TestTransferQueue::testResumeTransferOtherPresent() {
    auto path = QString("path");
    EXPECT_CALL(*_first, addToQueue())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_first, state())
        .Times(AnyNumber())
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::PAUSE))
        .WillOnce(Return(Transfer::PAUSE))
        .WillOnce(Return(Transfer::PAUSE))
        .WillOnce(Return(Transfer::PAUSE))
        .WillOnce(Return(Transfer::PAUSE))
        .WillOnce(Return(Transfer::RESUME));

    EXPECT_CALL(*_first, path())
        .Times(AnyNumber())
        .WillRepeatedly(Return(path));

    EXPECT_CALL(*_first, canTransfer())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_first, startTransfer())
        .Times(1);

    EXPECT_CALL(*_first, pauseTransfer())
        .Times(1);

    // second transfer expectations
    auto secondPath = QString("second path");
    EXPECT_CALL(*_second, addToQueue())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_second, state())
        .Times(AnyNumber())
        .WillRepeatedly(Return(Transfer::START));

    EXPECT_CALL(*_second, path())
        .Times(1)
        .WillRepeatedly(Return(secondPath));

    EXPECT_CALL(*_second, canTransfer())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_second, startTransfer())
        .Times(1);

    // add a transfer, set the state to start and assert that it will
    // be started
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->stateChanged();
    _second->stateChanged();
    _first->stateChanged();
    QCOMPARE(spy.count(), 2);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), path);
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), secondPath);
    verifyMocks();
}

void
TestTransferQueue::testResumeTransferNoOtherPresentCannotTransfer() {
    auto path = QString("path");
    EXPECT_CALL(*_first, addToQueue())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_first, state())
        .Times(4)
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::PAUSE))
        .WillOnce(Return(Transfer::RESUME))
        .WillOnce(Return(Transfer::RESUME));

    EXPECT_CALL(*_first, path())
        .Times(AnyNumber())
        .WillRepeatedly(Return(path));

    EXPECT_CALL(*_first, canTransfer())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_first, resumeTransfer())
        .Times(1);

    // second transfer expectations
    auto secondPath = QString("second path");
    EXPECT_CALL(*_second, addToQueue())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_second, state())
        .Times(AnyNumber())
        .WillRepeatedly(Return(Transfer::START));

    EXPECT_CALL(*_second, path())
        .Times(1)
        .WillRepeatedly(Return(secondPath));

    EXPECT_CALL(*_second, canTransfer())
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*_second, startTransfer())
        .Times(0);

    // add a transfer, set the state to start and assert that it will
    // be started
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->stateChanged();
    _second->stateChanged();
    _first->stateChanged();
    QCOMPARE(spy.count(), 2);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), QString());
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), path);
    verifyMocks();
}

void
TestTransferQueue::testCancelTransferNoOtherReady() {
    auto path = QString("path");
    EXPECT_CALL(*_first, addToQueue())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_first, state())
        .Times(4)
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::CANCEL))
        .WillOnce(Return(Transfer::CANCEL));

    EXPECT_CALL(*_first, path())
        .Times(2)
        .WillRepeatedly(Return(path));

    EXPECT_CALL(*_first, canTransfer())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_first, startTransfer())
        .Times(1);

    EXPECT_CALL(*_first, cancelTransfer())
        .Times(1);

    // add a transfer, set the state to start and assert that it will
    // be started
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->stateChanged();
    _first->stateChanged();
    QCOMPARE(spy.count(), 2);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), path);
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), QString());
    verifyMocks();
}

void
TestTransferQueue::testCancelTransferOtherReady() {
    auto path = QString("path");
    EXPECT_CALL(*_first, addToQueue())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_first, state())
        .Times(AnyNumber())
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::CANCEL))
        .WillOnce(Return(Transfer::CANCEL))
        .WillOnce(Return(Transfer::CANCEL))
        .WillOnce(Return(Transfer::CANCEL))
        .WillOnce(Return(Transfer::CANCEL));

    EXPECT_CALL(*_first, path())
        .Times(AnyNumber())
        .WillRepeatedly(Return(path));

    EXPECT_CALL(*_first, canTransfer())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_first, startTransfer())
        .Times(1);

    EXPECT_CALL(*_first, cancelTransfer())
        .Times(1);

    // second transfer expectations
    auto secondPath = QString("second path");
    EXPECT_CALL(*_second, addToQueue())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_second, state())
        .Times(AnyNumber())
        .WillRepeatedly(Return(Transfer::START));

    EXPECT_CALL(*_second, path())
        .Times(1)
        .WillRepeatedly(Return(secondPath));

    EXPECT_CALL(*_second, canTransfer())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_second, startTransfer())
        .Times(1);

    // add a transfer, set the state to start and assert that it will
    // be started
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->stateChanged();
    _second->stateChanged();
    _first->stateChanged();
    QCOMPARE(spy.count(), 2);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), path);
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), secondPath);
    verifyMocks();
}

void
TestTransferQueue::testCancelTransferOtherReadyCannotTransfer() {
    auto path = QString("path");
    EXPECT_CALL(*_first, addToQueue())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_first, state())
        .Times(AnyNumber())
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::CANCEL))
        .WillOnce(Return(Transfer::CANCEL))
        .WillOnce(Return(Transfer::CANCEL))
        .WillOnce(Return(Transfer::CANCEL))
        .WillOnce(Return(Transfer::CANCEL));

    EXPECT_CALL(*_first, path())
        .Times(AnyNumber())
        .WillRepeatedly(Return(path));

    EXPECT_CALL(*_first, canTransfer())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_first, startTransfer())
        .Times(1);

    EXPECT_CALL(*_first, cancelTransfer())
        .Times(1);

    // second transfer expectations
    auto secondPath = QString("second path");
    EXPECT_CALL(*_second, addToQueue())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_second, state())
        .Times(AnyNumber())
        .WillRepeatedly(Return(Transfer::START));

    EXPECT_CALL(*_second, path())
        .Times(1)
        .WillRepeatedly(Return(secondPath));

    EXPECT_CALL(*_second, canTransfer())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_second, startTransfer())
        .Times(1);

    // add a transfer, set the state to start and assert that it will
    // be started
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->stateChanged();
    _second->stateChanged();
    _first->stateChanged();
    QCOMPARE(spy.count(), 2);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), path);
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), secondPath);
    verifyMocks();
}

void
TestTransferQueue::testCancelTransferNotStarted() {
    auto path = QString("path");
    EXPECT_CALL(*_first, addToQueue())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_first, state())
        .Times(1)
        .WillRepeatedly(Return(Transfer::CANCEL));

    EXPECT_CALL(*_first, path())
        .Times(2)
        .WillRepeatedly(Return(path));

    EXPECT_CALL(*_first, startTransfer())
        .Times(0);

    EXPECT_CALL(*_first, cancelTransfer())
        .Times(1);

    // cancel not started and ensure that it is removed
    QSignalSpy removedSpy(_q, SIGNAL(transferRemoved(QString)));
    _q->add(_first);

    QVERIFY(_q->currentTransfer().isEmpty());

    _first->stateChanged();

    QVERIFY(_q->currentTransfer().isEmpty());
}

void
TestTransferQueue::testTransfers() {
    auto path = QString("path");
    EXPECT_CALL(*_first, addToQueue())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_first, path())
        .Times(2)
        .WillRepeatedly(Return(path));

    auto secondPath = QString("second path");
    EXPECT_CALL(*_second, addToQueue())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_second, path())
        .Times(2)
        .WillRepeatedly(Return(secondPath));

    // add the transfers to the q and assert that they are all returned
    _q->add(_first);
    _q->add(_second);

    QHash<QString, Transfer*> transfers = _q->transfers();
    QCOMPARE(transfers[_first->path()], _first);
    QCOMPARE(transfers[_second->path()], _second);
}

void
TestTransferQueue::testTransferFinishedOtherReady() {
    auto path = QString("path");
    EXPECT_CALL(*_first, addToQueue())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_first, state())
        .Times(AnyNumber())
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::FINISH))
        .WillOnce(Return(Transfer::FINISH))
        .WillOnce(Return(Transfer::FINISH))
        .WillOnce(Return(Transfer::FINISH))
        .WillOnce(Return(Transfer::FINISH));

    EXPECT_CALL(*_first, path())
        .Times(AnyNumber())
        .WillRepeatedly(Return(path));

    EXPECT_CALL(*_first, canTransfer())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_first, startTransfer())
        .Times(1);

    // second transfer expectations
    auto secondPath = QString("second path");
    EXPECT_CALL(*_second, addToQueue())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_second, state())
        .Times(AnyNumber())
        .WillRepeatedly(Return(Transfer::START));

    EXPECT_CALL(*_second, path())
        .Times(1)
        .WillRepeatedly(Return(secondPath));

    EXPECT_CALL(*_second, canTransfer())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_second, startTransfer())
        .Times(1);

    // add a transfer, set the state to start and assert that it will
    // be started
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->stateChanged();
    _second->stateChanged();
    _first->stateChanged();
    QCOMPARE(spy.count(), 2);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), path);
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), secondPath);
    verifyMocks();
}

void
TestTransferQueue::testTransferErrorWithOtherReady() {
    auto path = QString("path");
    EXPECT_CALL(*_first, addToQueue())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_first, state())
        .Times(AnyNumber())
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::START))
        .WillOnce(Return(Transfer::ERROR))
        .WillOnce(Return(Transfer::ERROR))
        .WillOnce(Return(Transfer::ERROR))
        .WillOnce(Return(Transfer::ERROR))
        .WillOnce(Return(Transfer::ERROR));

    EXPECT_CALL(*_first, path())
        .Times(AnyNumber())
        .WillRepeatedly(Return(path));

    EXPECT_CALL(*_first, canTransfer())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_first, startTransfer())
        .Times(1);

    // second transfer expectations
    auto secondPath = QString("second path");
    EXPECT_CALL(*_second, addToQueue())
        .Times(AnyNumber())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_second, state())
        .Times(AnyNumber())
        .WillRepeatedly(Return(Transfer::START));

    EXPECT_CALL(*_second, path())
        .Times(1)
        .WillRepeatedly(Return(secondPath));

    EXPECT_CALL(*_second, canTransfer())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_second, startTransfer())
        .Times(1);

    // add a transfer, set the state to start and assert that it will
    // be started
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not transfer just yet
    QVERIFY(_q->currentTransfer().isEmpty());

    _first->stateChanged();
    _second->stateChanged();
    _first->stateChanged();
    QCOMPARE(spy.count(), 2);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), path);
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), secondPath);
    verifyMocks();
}

void
TestTransferQueue::testNewUnmanagedIncreasesNumber() {
    EXPECT_CALL(*_first, addToQueue())
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*_first, path())
        .Times(1)
        .WillRepeatedly(Return(QString("path")));

    EXPECT_CALL(*_second, addToQueue())
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*_second, path())
        .Times(1)
        .WillRepeatedly(Return(QString("second path")));

    _q->add(_first);

    QCOMPARE(1, _q->size());

    _q->add(_second);
    QCOMPARE(2, _q->size());
}

void
TestTransferQueue::testErrorUnmanagedDecreasesNumber() {
    EXPECT_CALL(*_first, addToQueue())
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*_first, path())
        .Times(2)
        .WillRepeatedly(Return(QString("path")));

    EXPECT_CALL(*_first, state())
        .Times(1)
        .WillOnce(Return(Transfer::ERROR));
    QSignalSpy spy(_q, SIGNAL(transferRemoved(QString)));

    _q->add(_first);

    QCOMPARE(1, _q->size());
    _first->stateChanged();

    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(0, _q->size());
}

void
TestTransferQueue::testFinishUnmanagedDecreasesNumber() {
    EXPECT_CALL(*_first, addToQueue())
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*_first, path())
        .Times(2)
        .WillRepeatedly(Return(QString("path")));

    EXPECT_CALL(*_first, state())
        .Times(1)
        .WillOnce(Return(Transfer::FINISH));
    QSignalSpy spy(_q, SIGNAL(transferRemoved(QString)));

    _q->add(_first);

    QCOMPARE(1, _q->size());
    _first->stateChanged();

    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(0, _q->size());
}

void
TestTransferQueue::testCancelUnmanagedDecreasesNumber() {
    EXPECT_CALL(*_first, addToQueue())
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*_first, path())
        .Times(2)
        .WillRepeatedly(Return(QString("path")));

    EXPECT_CALL(*_first, state())
        .Times(1)
        .WillOnce(Return(Transfer::CANCEL));

    QSignalSpy spy(_q, SIGNAL(transferRemoved(QString)));

    _q->add(_first);
    QCOMPARE(1, _q->size());
    _first->stateChanged();

    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(0, _q->size());
    verifyMocks();
}


QTEST_MAIN(TestTransferQueue)

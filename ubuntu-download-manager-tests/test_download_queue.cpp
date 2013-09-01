/*
 * Copyright 2013 2013 Canonical Ltd.
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
#include <QSignalSpy>
#include "./test_download_queue.h"

TestDownloadQueue::TestDownloadQueue(QObject *parent)
    : QObject(parent) {
}

void
TestDownloadQueue::init() {
    _networkInfo = new FakeSystemNetworkInfo();
    _reqFactory = new FakeRequestFactory();
    _processFactory = new FakeProcessFactory();
    _first = new FakeDownload(QUuid::createUuid(), "first-path", QUrl(),
        QVariantMap(), QMap<QString, QString>(), _networkInfo, _reqFactory,
        _processFactory);
    _second = new FakeDownload(QUuid::createUuid(), "second-path", QUrl(),
        QVariantMap(), QMap<QString, QString>(), _networkInfo, _reqFactory,
        _processFactory);
    _q = new DownloadQueue(_networkInfo);
}

void
TestDownloadQueue::cleanup() {
    if (_q != NULL)
        delete _q;

    if (_reqFactory != NULL)
        delete _reqFactory;

    if (_first != NULL)
        delete _first;

    if (_second != NULL)
        delete _second;
}

void
TestDownloadQueue::testAddDownload() {
    // test that when a download added the add signals is raised
    QSignalSpy spy(_q, SIGNAL(downloadAdded(QString)));
    _q->add(_first);

    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());
}

void
TestDownloadQueue::testStartDownloadWithNoCurrent() {
    // add a download, set the state to start and assert that it will be started
    _first->record();
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);

    // we do not download just yet
    QVERIFY(_q->currentDownload().isEmpty());

    _first->start();  // emit signal
    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());

    // state that startDownload was called in first
    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
    QCOMPARE(QString("startDownload"), calledMethods[1].methodName());
}

void
TestDownloadQueue::testStartDownloadWithCurrent() {
    // add a download, start it, add a second one and
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

    // state that startDownload was called in first
    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
    QCOMPARE(QString("startDownload"), calledMethods[1].methodName());

    // state that startDownload was NOT called in second
    calledMethods = _second->calledMethods();
    QCOMPARE(0, calledMethods.count());
}

void
TestDownloadQueue::testStartDownloadWithNoCurrentCannotDownload() {
    // tell the fake that it cannot download (GSM enabled)
    _first->setCanDownload(false);
    _first->record();
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);

    // we do not download just yet
    QVERIFY(_q->currentDownload().isEmpty());

    _first->start();  // emit signal
    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), QString(""));

    // state that startDownload was called in first
    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(1, calledMethods.count());  // just canDownload should be called
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
}

void
TestDownloadQueue::testPauseDownloadNoOtherReady() {
    // add, start and pause a download, this should,
    // chage current to "" and emit the required
    // signals and execute the required methods
    _first->record();
    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);

    QVERIFY(_q->currentDownload().isEmpty());

    _first->start();
    _first->pause();
    QVERIFY(_q->currentDownload().isEmpty());

    QCOMPARE(spy.count(), 2);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());
    arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString().isEmpty());

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(5, calledMethods.count());
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
    QCOMPARE(QString("startDownload"), calledMethods[1].methodName());
    QCOMPARE(QString("pauseDownload"), calledMethods[2].methodName());
    QCOMPARE(QString("canDownload"), calledMethods[3].methodName());
}

void
TestDownloadQueue::testPauseDownloadOtherReady() {
    // add two downloads, start them, pause the first
    // and assert that the second one is started
    _first->record();
    _second->record();

    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not download just yet
    QVERIFY(_q->currentDownload().isEmpty());

    _first->start();
    _second->start();
    _first->pause();
    QCOMPARE(_q->currentDownload(), _second->path());

    QCOMPARE(spy.count(), 2);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _second->path());

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(5, calledMethods.count());
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
    QCOMPARE(QString("startDownload"), calledMethods[1].methodName());
    QCOMPARE(QString("pauseDownload"), calledMethods[2].methodName());

    calledMethods = _second->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
    QCOMPARE(QString("startDownload"), calledMethods[1].methodName());
}

void
TestDownloadQueue::testResumeDownloadNoOtherPresent() {
    // add, start, pause, resume and assert that the same guy
    // is used even when other paused downloads are present
    _first->record();
    _second->record();

    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not download just yet
    QVERIFY(_q->currentDownload().isEmpty());

    _first->start();
    _first->pause();
    _first->resume();

    QCOMPARE(_q->currentDownload(), _first->path());

    QCOMPARE(spy.count(), 3);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());
    arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString().isEmpty());
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(7, calledMethods.count());
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
    QCOMPARE(QString("startDownload"), calledMethods[1].methodName());
    QCOMPARE(QString("pauseDownload"), calledMethods[2].methodName());
    QCOMPARE(QString("canDownload"), calledMethods[3].methodName());
    QCOMPARE(QString("canDownload"), calledMethods[4].methodName());
    QCOMPARE(QString("canDownload"), calledMethods[5].methodName());
    QCOMPARE(QString("resumeDownload"), calledMethods[6].methodName());

    calledMethods = _second->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
}

void
TestDownloadQueue::testResumeDownloadOtherPresent() {
    // add tow, start, pause, resume and assert that
    // resume does not change the current one
    _first->record();
    _second->record();

    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not download just yet
    QVERIFY(_q->currentDownload().isEmpty());

    _first->start();
    _first->pause();
    _second->start();
    _first->resume();

    QCOMPARE(_q->currentDownload(), _second->path());

    QCOMPARE(spy.count(), 3);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());
    arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString().isEmpty());
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _second->path());

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(6, calledMethods.count());
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
    QCOMPARE(QString("startDownload"), calledMethods[1].methodName());
    QCOMPARE(QString("pauseDownload"), calledMethods[2].methodName());
    QCOMPARE(QString("canDownload"), calledMethods[3].methodName());
    QCOMPARE(QString("canDownload"), calledMethods[4].methodName());

    calledMethods = _second->calledMethods();
    QCOMPARE(3, calledMethods.count());
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
    QCOMPARE(QString("canDownload"), calledMethods[1].methodName());
    QCOMPARE(QString("startDownload"), calledMethods[2].methodName());
}

void
TestDownloadQueue::testResumeDownloadNoOtherPresentCannotDownload() {
    // add, start, pause, resume and assert that the same guy
    // is used even when other paused downloads are present
    _first->record();
    _second->record();

    QSignalSpy spy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not download just yet
    QVERIFY(_q->currentDownload().isEmpty());

    _first->start();
    _first->pause();
    _first->setCanDownload(false);
    _first->resume();

    QCOMPARE(_q->currentDownload(), QString(""));

    QCOMPARE(spy.count(), 3);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());
    arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString().isEmpty());
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), QString(""));

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(6, calledMethods.count());  // only canDownload start and pause
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
    QCOMPARE(QString("startDownload"), calledMethods[1].methodName());
    QCOMPARE(QString("pauseDownload"), calledMethods[2].methodName());
    QCOMPARE(QString("canDownload"), calledMethods[3].methodName());

    calledMethods = _second->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
    QCOMPARE(QString("canDownload"), calledMethods[1].methodName());
}

void
TestDownloadQueue::testCancelDownloadNoOtherReady() {
    // cancel the download and expect it to be done
    _first->record();
    QSignalSpy changedSpy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);

    QVERIFY(_q->currentDownload().isEmpty());

    _first->start();
    _first->cancel();
    QVERIFY(_q->currentDownload().isEmpty());

    QCOMPARE(changedSpy.count(), 2);

    QList<QVariant> arguments = changedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());
    arguments = changedSpy.takeFirst();
    QVERIFY(arguments.at(0).toString().isEmpty());

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(3, calledMethods.count());
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
    QCOMPARE(QString("startDownload"), calledMethods[1].methodName());
    QCOMPARE(QString("cancelDownload"), calledMethods[2].methodName());
}

void
TestDownloadQueue::testCancelDownloadOtherReady() {
    // start and cancel and assert the second one is started
    _first->record();
    _second->record();

    QSignalSpy changedSpy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not download just yet
    QVERIFY(_q->currentDownload().isEmpty());

    _first->start();
    _second->start();
    _first->cancel();
    QCOMPARE(_q->currentDownload(), _second->path());

    QCOMPARE(changedSpy.count(), 2);

    QList<QVariant> arguments = changedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());
    arguments = changedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _second->path());

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(3, calledMethods.count());
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
    QCOMPARE(QString("startDownload"), calledMethods[1].methodName());
    QCOMPARE(QString("cancelDownload"), calledMethods[2].methodName());

    calledMethods = _second->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
    QCOMPARE(QString("startDownload"), calledMethods[1].methodName());
}

void
TestDownloadQueue::testCancelDownloadOtherReadyCannotDownload() {
    // start and cancel and assert the second one is not started
    _first->record();
    _second->setCanDownload(false);
    _second->record();

    QSignalSpy changedSpy(_q, SIGNAL(currentChanged(QString)));
    _q->add(_first);
    _q->add(_second);

    // we do not download just yet
    QVERIFY(_q->currentDownload().isEmpty());

    _first->start();
    _second->start();
    _first->cancel();
    QCOMPARE(_q->currentDownload(), QString(""));

    QCOMPARE(changedSpy.count(), 2);

    QList<QVariant> arguments = changedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), _first->path());
    arguments = changedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), QString(""));

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(3, calledMethods.count());
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
    QCOMPARE(QString("startDownload"), calledMethods[1].methodName());
    QCOMPARE(QString("cancelDownload"), calledMethods[2].methodName());

    calledMethods = _second->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
}

void
TestDownloadQueue::testCancelDownloadNotStarted() {
    // cancel not started and ensure that it is removed
    _first->record();
    QSignalSpy removedSpy(_q, SIGNAL(downloadRemoved(QString)));
    _q->add(_first);

    QVERIFY(_q->currentDownload().isEmpty());

    _first->cancel();
    QVERIFY(_q->currentDownload().isEmpty());
}

void
TestDownloadQueue::testDownloads() {
    // add the downloads to the q and assert that they are all returned
    _q->add(_first);
    _q->add(_second);

    QHash<QString, Download*> downloads = _q->downloads();
    QCOMPARE(downloads[_first->path()], _first);
    QCOMPARE(downloads[_second->path()], _second);
}

void
TestDownloadQueue::testDownloadFinishedOtherReady() {
    _first->record();
    _second->record();

    _q->add(_first);
    _q->add(_second);

    _first->start();
    _second->start();
    _first->emitFinished("");

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
    QCOMPARE(QString("startDownload"), calledMethods[1].methodName());

    calledMethods = _second->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
    QCOMPARE(QString("startDownload"), calledMethods[1].methodName());
}

void
TestDownloadQueue::testDownloadErrorWithOtherReady() {
    _first->record();
    _second->record();

    _q->add(_first);
    _q->add(_second);

    _first->start();
    _second->start();
    _first->emitError("");

    QList<MethodData> calledMethods = _first->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
    QCOMPARE(QString("startDownload"), calledMethods[1].methodName());

    calledMethods = _second->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(QString("canDownload"), calledMethods[0].methodName());
    QCOMPARE(QString("startDownload"), calledMethods[1].methodName());
}

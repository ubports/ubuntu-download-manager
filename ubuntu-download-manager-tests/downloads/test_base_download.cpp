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

#include "test_base_download.h"

TestBaseDownload::TestBaseDownload(QObject *parent)
    : BaseTestCase("TestBaseDownload", parent){
}

void
TestBaseDownload::init() {
    BaseTestCase::init();

    _id = "download-id";
    _path = "path-to-be-used";
    _isConfined = false;
    _rootPath = "/root/path/to/use";
    _url = QUrl("http://test.com");
}

void
TestBaseDownload::testStartQueued() {
    QScopedPointer<FakeDownload> down(
        new FakeDownload(_id, _path, _isConfined, _rootPath, _url,
            _metadata, _headers));
    down->record();
    down->start();

    QCOMPARE(0, down->calledMethods().count());
}

void
TestBaseDownload::testStartNotQueued() {
    QScopedPointer<FakeDownload> down(
        new FakeDownload(_id, _path, _isConfined, _rootPath, _url,
            _metadata, _headers));
    down->record();
    down->setAddToQueue(false);
    down->start();

    QCOMPARE(1, down->calledMethods().count());
    QCOMPARE(QString("startTransfer"), down->calledMethods()[0].methodName());
}

void
TestBaseDownload::testPauseQueued() {
    QScopedPointer<FakeDownload> down(
        new FakeDownload(_id, _path, _isConfined, _rootPath, _url,
            _metadata, _headers));
    down->record();
    down->pause();

    QCOMPARE(0, down->calledMethods().count());
}

void
TestBaseDownload::testPauseNotQueued() {
    QScopedPointer<FakeDownload> down(
        new FakeDownload(_id, _path, _isConfined, _rootPath, _url,
            _metadata, _headers));
    down->record();
    down->setAddToQueue(false);
    down->pause();

    QCOMPARE(1, down->calledMethods().count());
    QCOMPARE(QString("pauseTransfer"), down->calledMethods()[0].methodName());
}

void
TestBaseDownload::testResumeQueued() {
    QScopedPointer<FakeDownload> down(
        new FakeDownload(_id, _path, _isConfined, _rootPath, _url,
            _metadata, _headers));
    down->record();
    down->resume();

    QCOMPARE(0, down->calledMethods().count());
}

void
TestBaseDownload::testResumeNotQueued() {
    QScopedPointer<FakeDownload> down(
        new FakeDownload(_id, _path, _isConfined, _rootPath, _url,
            _metadata, _headers));
    down->record();
    down->setAddToQueue(false);
    down->resume();

    QCOMPARE(1, down->calledMethods().count());
    QCOMPARE(QString("resumeTransfer"), down->calledMethods()[0].methodName());
}

void
TestBaseDownload::testCancelQueued() {
    QScopedPointer<FakeDownload> down(
        new FakeDownload(_id, _path, _isConfined, _rootPath, _url,
            _metadata, _headers));
    down->record();
    down->cancel();

    QCOMPARE(0, down->calledMethods().count());
}

void
TestBaseDownload::testCancelNotQueued() {
    QScopedPointer<FakeDownload> down(
        new FakeDownload(_id, _path, _isConfined, _rootPath, _url,
            _metadata, _headers));
    down->record();
    down->setAddToQueue(false);
    down->cancel();

    QCOMPARE(1, down->calledMethods().count());
    QCOMPARE(QString("cancelTransfer"), down->calledMethods()[0].methodName());
}

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

#include "test_group_watch.h"

TestGroupWatch::TestGroupWatch(QObject* parent)
    : DaemonTestCase("TestGroupWatch", parent) {
}

void
TestGroupWatch::onSuccessCb(GroupDownload* down) {
    _calledSuccess = true;
    delete down;
    emit callbackExecuted();
}

void
TestGroupWatch::onErrorCb(GroupDownload* err) {
    _calledError = true;
    delete err;
    emit errbackExecuted();
}

void
TestGroupWatch::init() {
    DaemonTestCase::init();
    _calledSuccess = false;
    _calledError = false;
    _algo = "Md5";
    _manager = Manager::createSessionManager(daemonPath(), this);
}

void
TestGroupWatch::cleanup() {
    delete _manager;
    DaemonTestCase::cleanup();
}

void
TestGroupWatch::testCallbackIsExecuted() {
    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        "local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    GroupCb cb = std::bind(&TestGroupWatch::onSuccessCb, this,
        std::placeholders::_1);
    GroupCb errCb = std::bind(&TestGroupWatch::onErrorCb, this,
        std::placeholders::_1);

    QEventLoop loop;
    QObject::connect(this, &TestGroupWatch::callbackExecuted,
        &loop, &QEventLoop::quit);

    _manager->createDownload(downloadsStruct, _algo, false, _metadata, _headers,
        cb, errCb);
    loop.exec();

    QVERIFY(_calledSuccess);
    QVERIFY(!_calledError);
}

void
TestGroupWatch::testErrCallbackIsExecuted() {
    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        "local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "local_file", ""));  // same local path that raises an error
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    GroupCb cb = std::bind(&TestGroupWatch::onSuccessCb, this,
        std::placeholders::_1);
    GroupCb errCb = std::bind(&TestGroupWatch::onErrorCb, this,
        std::placeholders::_1);

    QEventLoop loop;
    QObject::connect(this, &TestGroupWatch::errbackExecuted,
        &loop, &QEventLoop::quit);

    _manager->createDownload(downloadsStruct, _algo, false, _metadata, _headers,
        cb, errCb);
    loop.exec();

    QVERIFY(!_calledSuccess);
    QVERIFY(_calledError);
}

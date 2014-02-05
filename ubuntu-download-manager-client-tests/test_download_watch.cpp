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

#include <QScopedPointer>
#include <QSignalSpy>
#include <ubuntu/download_manager/download.h>
#include "test_download_watch.h"

TestDownloadWatch::TestDownloadWatch(QObject *parent)
    : LocalTreeTestCase("TestDownloadWatch", parent) {
}

void
TestDownloadWatch::init() {
    LocalTreeTestCase::init();
    _manager = Manager::createSessionManager(daemonPath(), this);
}

void
TestDownloadWatch::cleanup() {
    delete _manager;
    LocalTreeTestCase::cleanup();
}

void
TestDownloadWatch::testErrorRaised() {
    // create a download with a missing url in the local host to ensure
    // that the error signal is raised
    QVariantMap metadata;
    QMap<QString, QString> headers;
    QUrl notPresentFile = serverUrl();
    DownloadStruct downStruct(notPresentFile.toString() + "/not_present.zip",
        metadata, headers);

    // use the blocking call so that we get a download
    QSignalSpy managerSpy(_manager, SIGNAL(downloadCreated(Download*)));
    _manager->createDownload(downStruct);

    QTRY_COMPARE(1, managerSpy.count());
    QScopedPointer<Download> down(managerSpy.takeFirst().at(0).value<Download*>());

    QSignalSpy spy(down.data(), SIGNAL(error(Error*)));
    returnDBusErrors(true);

    down->start();
    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 1, 10000);
}

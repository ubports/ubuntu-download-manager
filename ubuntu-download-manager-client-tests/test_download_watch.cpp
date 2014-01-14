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
    notPresentFile.setPath("not/present.zip");
    DownloadStruct downStruct(notPresentFile.toString(), metadata, headers);

    // use the blocking call so that we get a download
    QScopedPointer<Download> down(_manager->createDownload(downStruct));
    QSignalSpy spy(down.data(), SIGNAL(error(Error*)));
    returnDBusErrors(true);

    down->setThrottle(0);
    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 1, 10000);
}

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
#include "error.h"
#include "test_downloads_list.h"

TestDownloadsList::TestDownloadsList(QObject *parent)
    : LocalTreeTestCase("TestDownloadsList", parent){
}

void
TestDownloadsList::testIsError() {
    TestingError* err = new TestingError(
        Ubuntu::DownloadManager::Error::DBus);
    QScopedPointer<DownloadsListImpl> errList(new DownloadsListImpl(err));
    QVERIFY(errList->isError());
}

void
TestDownloadsList::testIsErrorEmptyList() {
    QScopedPointer<DownloadsListImpl> list(new DownloadsListImpl());
    QVERIFY(!list->isError());
}

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

#ifndef FAKE_DOWNLOAD_QUEUE_H
#define FAKE_DOWNLOAD_QUEUE_H

#include <ubuntu/downloads/queue.h>
#include <gmock/gmock.h>

using namespace Ubuntu::DownloadManager::Daemon;

class MockDownloadQueue : public Queue {
 public:
    MOCK_METHOD1(add, void(Download*));
    MOCK_METHOD0(currentDownload, QString());
    MOCK_METHOD0(paths, QStringList());
    MOCK_METHOD0(downloads, QHash<QString, Download*>());
    MOCK_METHOD0(size, int());

    using Queue::currentChanged;
    using Queue::downloadAdded;
    using Queue::downloadRemoved;
};

#endif  // FAKE_DOWNLOAD_QUEUE_H

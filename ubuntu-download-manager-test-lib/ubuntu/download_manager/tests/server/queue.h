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

#ifndef FAKE_DOWNLOAD_QUEUE_H
#define FAKE_DOWNLOAD_QUEUE_H
#include <QSharedPointer>
#include <downloads/queue.h>
#include <system/system_network_info.h>
#include "ubuntu/download_manager/tests/fake.h"

using namespace Ubuntu::DownloadManager::Daemon;

class FakeDownloadQueue : public Queue, public Fake {
    Q_OBJECT

 public:
    explicit FakeDownloadQueue(QObject *parent = 0);

    void add(Download* download) override;
    int size() override;
    void setSize(int size);

    void emitDownloadAdded(const QString& path);
    void emitDownloadRemoved(const QString& path);

 private:
    int _size;
};

#endif  // FAKE_DOWNLOAD_QUEUE_H

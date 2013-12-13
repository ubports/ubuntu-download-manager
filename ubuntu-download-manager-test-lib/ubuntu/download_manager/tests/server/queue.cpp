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

#include "queue.h"

FakeDownloadQueue::FakeDownloadQueue(QObject *parent)
    : Queue(parent),
    Fake() {
}

void
FakeDownloadQueue::add(Download* download) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(download);

        QList<QObject*> outParams;

        MethodParams params(inParams, outParams);

        MethodData methodData("add", params);
        _called.append(methodData);
    }
    Queue::add(download);
}

int
FakeDownloadQueue::size() {
    return _size;
}

void
FakeDownloadQueue::setSize(int size) {
    _size = size;
}

void
FakeDownloadQueue::emitDownloadAdded(const QString& path) {
    emit downloadAdded(path);
}

void
FakeDownloadQueue::emitDownloadRemoved(const QString& path) {
    emit downloadRemoved(path);
}

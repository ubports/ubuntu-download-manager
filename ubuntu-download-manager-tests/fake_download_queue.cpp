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

#include "./fake_download_queue.h"

FakeDownloadQueue::FakeDownloadQueue(SystemNetworkInfo* networkInfo,
                                     QObject *parent)
    : DownloadQueue(networkInfo, parent),
    Fake() {
}

void
FakeDownloadQueue::add(Download* download, DownloadAdaptor* adaptor) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(download);
        inParams.append(adaptor);

        QList<QObject*> outParams;

        MethodParams params(inParams, outParams);

        MethodData methodData("add", params);
        _called.append(methodData);
    }
    DownloadQueue::add(download, adaptor);
}

void
FakeDownloadQueue::add(const QPair<Download*, DownloadAdaptor*>& value) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(value.first);
        inParams.append(value.second);

        QList<QObject*> outParams;

        MethodParams params(inParams, outParams);

        MethodData methodData("add", params);
        _called.append(methodData);
    }
    DownloadQueue::add(value);
}

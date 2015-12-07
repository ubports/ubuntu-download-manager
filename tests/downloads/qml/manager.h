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

#ifndef FAKE_MANAGER_DOWNLOAD_H
#define FAKE_MANAGER_DOWNLOAD_H

#include <ubuntu/download_manager/manager.h>
#include <gmock/gmock.h>

using namespace Ubuntu::DownloadManager;

class MockManager : public Manager {
 public:
    explicit MockManager(QObject* parent=0)
        : Manager(parent) {
    }

    MOCK_METHOD1(getDownloadForId, Download*(const QString&));
    MOCK_METHOD1(createDownload, void(DownloadStruct));
    MOCK_METHOD3(createDownload, void(DownloadStruct, DownloadCb, DownloadCb));
    MOCK_METHOD5(createDownload, void(StructList, const QString&, bool,
        const QVariantMap&, StringMap));
    MOCK_METHOD7(createDownload, void(StructList, const QString&, bool,
        const QVariantMap&, StringMap, GroupCb, GroupCb));
    MOCK_METHOD0(getAllDownloads, void(const QString&, bool));
    MOCK_METHOD2(getAllDownloads, void(const QString&, bool, DownloadsListCb, DownloadsListCb));
    MOCK_METHOD2(getAllDownloadsWithMetadata, void(const QString&,
        const QString&));
    MOCK_METHOD4(getAllDownloadsWithMetadata, void(const QString&,
        const QString&, MetadataDownloadsListCb, MetadataDownloadsListCb));
    MOCK_CONST_METHOD0(isError, bool());
    MOCK_CONST_METHOD0(lastError, Error*());
    MOCK_METHOD1(allowMobileDataDownload, void(bool));
    MOCK_METHOD0(isMobileDataDownload, bool());
    MOCK_METHOD0(defaultThrottle, qulonglong());
    MOCK_METHOD1(setDefaultThrottle, void(qulonglong));
    MOCK_METHOD0(exit, void());
};

#endif

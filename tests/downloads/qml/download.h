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

#ifndef FAKE_CLIENT_DOWNLOAD_H
#define FAKE_CLIENT_DOWNLOAD_H

#include <ubuntu/download_manager/download.h>
#include <gmock/gmock.h>

using namespace Ubuntu::DownloadManager;

class MockDownload : public Download {

 public:
     explicit MockDownload(QObject* parent=0)
         : Download(parent) {
     }

    MOCK_METHOD0(start, void());
    MOCK_METHOD0(pause, void());
    MOCK_METHOD0(resume, void());
    MOCK_METHOD0(cancel, void());
    MOCK_METHOD0(collected, void());
    MOCK_METHOD1(allowMobileDownload, void(bool));
    MOCK_METHOD0(isMobileDownloadAllowed, bool());
    MOCK_METHOD1(setDestinationDir, void(const QString&));
    MOCK_METHOD1(setHeaders, void(QMap<QString, QString> headers));
    MOCK_METHOD1(setMetadata, void(QVariantMap));
    MOCK_METHOD0(headers, QMap<QString, QString>());
    MOCK_METHOD1(setThrottle, void(qulonglong));
    MOCK_METHOD0(throttle, qulonglong());
    MOCK_CONST_METHOD0(id, QString());
    MOCK_METHOD0(metadata, QVariantMap());
    MOCK_METHOD0(progress, qulonglong());
    MOCK_METHOD0(totalSize, qulonglong());
    MOCK_METHOD0(filePath, QString());
    MOCK_METHOD0(state, State());
    MOCK_CONST_METHOD0(isError, bool());
    MOCK_CONST_METHOD0(error, Error*());
    MOCK_CONST_METHOD0(clickPackage, QString());
    MOCK_CONST_METHOD0(showInIndicator, bool());
    MOCK_CONST_METHOD0(title, QString());
    MOCK_CONST_METHOD0(destinationApp, QString());

    // expose the signals so that they can  emitted by external objects

    using Download::canceled;
    using Download::error;
    using Download::finished;
    using Download::paused;
    using Download::processing;
    using Download::progress;
    using Download::resumed;
    using Download::started;
    using Download::clickPackagedChanged;
    using Download::showInIndicatorChanged;
    using Download::titleChanged;
    using Download::destinationAppChanged;
};

#endif

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

#ifndef FAKE_SM_FILED_DOWNLOAD_H
#define FAKE_SM_FILED_DOWNLOAD_H

#include <QObject>
#include <QSslError>
#include <ubuntu/downloads/sm_file_download.h>
#include <gmock/gmock.h>

using namespace Ubuntu::DownloadManager::Daemon;

class MockSMFileDownload : public SMFileDownload {
 public:
    MOCK_METHOD1(emitError, void(QString));
    MOCK_METHOD1(emitNetworkError, void(QNetworkReply::NetworkError));
    MOCK_METHOD1(emitSslError, void(const QList<QSslError>&));
    MOCK_METHOD1(setState, void(Download::State));
    MOCK_METHOD0(requestDownload, void());
    MOCK_METHOD0(pauseRequestDownload, void());
    MOCK_METHOD0(cancelRequestDownload, void());

    using SMFileDownload::headRequestCompleted;
    using SMFileDownload::error;
    using SMFileDownload::sslErrors;
    using SMFileDownload::connectionEnabled;
    using SMFileDownload::connectionDisabled;
    using SMFileDownload::downloadingStarted;
    using SMFileDownload::paused;
    using SMFileDownload::completed;
    using SMFileDownload::hashingStarted;
    using SMFileDownload::hashingError;
    using SMFileDownload::postProcessingStarted;
    using SMFileDownload::postProcessingError;
    using SMFileDownload::finished;
    using SMFileDownload::canceled;
};

#endif  // FAKE_SM_FILED_DOWNLOAD_H

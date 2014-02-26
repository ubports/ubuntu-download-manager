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

#ifndef TEST_MMS_DOWNLOAD_H
#define TEST_MMS_DOWNLOAD_H

#include <QObject>
#include <ubuntu/transfers/system/request_factory.h>
#include <ubuntu/transfers/system/apn_request_factory.h>
#include <ubuntu/downloads/mms_file_download.h>
#include <ubuntu/download_manager/tests/base_testcase.h>
#include <ubuntu/download_manager/tests/test_runner.h>


using namespace Ubuntu::System;
using namespace Ubuntu::DownloadManager::Daemon;

class PublicMmsFileDownload : public MmsFileDownload {
    Q_OBJECT

 public:
    PublicMmsFileDownload(const QString& id,
                          const QString& path,
                          bool isConfined,
                          const QString& rootPath,
                          const QUrl& url,
                          const QVariantMap& metadata,
                          const QMap<QString, QString>& headers,
                          const QNetworkProxy& proxy,
                          QObject* parent = 0)
        : MmsFileDownload(id, path, isConfined, rootPath, url, metadata,
              headers, proxy, parent) {
    }

    RequestFactory* nam() {
        return _requestFactory;
    }
};

class TestMmsDownload : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestMmsDownload(QObject *parent = 0);

 private slots:  // NOLINT(whitespace/indent)

    void testNetworkAccessManager();
    void testAddToQueue();

};

DECLARE_TEST(TestMmsDownload)

#endif // TEST_MMS_DOWNLOAD_H

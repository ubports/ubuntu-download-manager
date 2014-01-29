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

#include "test_mms_download.h"

TestMmsDownload::TestMmsDownload(QObject* parent)
    : BaseTestCase("TestMmsDownload", parent) {
}

void
TestMmsDownload::testNetworkAccessManager() {
    QString id = "id of the download";
    QString path = "my-file";
    bool isConfined = false;
    QString rootPath = "/root/path/to/use";
    QUrl url("http://example.com");
    QVariantMap metadata;
    QMap<QString, QString> headers;
    QString hostname = "http://example.com";
    int port = 80;
    QString username = "username";
    QString password = "password";
    QNetworkProxy proxy(QNetworkProxy::HttpProxy, hostname,
        port, username, password);

    QScopedPointer<PublicMmsFileDownload> down(
        new PublicMmsFileDownload(id, path, isConfined, rootPath,
            url, metadata, headers, proxy));
    ApnRequestFactory* nam = qobject_cast<ApnRequestFactory*>(down->nam());
    QVERIFY(nam != nullptr);
}

void
TestMmsDownload::testAddToQueue() {
    QString id = "id of the download";
    QString path = "my-file";
    bool isConfined = false;
    QString rootPath = "/root/path/to/use";
    QUrl url("http://example.com");
    QVariantMap metadata;
    QMap<QString, QString> headers;
    QString hostname = "http://example.com";
    int port = 80;
    QString username = "username";
    QString password = "password";
    QNetworkProxy proxy(QNetworkProxy::HttpProxy, hostname,
        port, username, password);

    QScopedPointer<PublicMmsFileDownload> down(
        new PublicMmsFileDownload(id, path, isConfined, rootPath,
            url, metadata, headers, proxy));
    QVERIFY(!down->addToQueue());
}

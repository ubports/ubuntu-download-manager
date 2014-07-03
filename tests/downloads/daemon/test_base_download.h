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

#ifndef TEST_BASE_DOWNLOAD_H
#define TEST_BASE_DOWNLOAD_H

#include <QMap>
#include <QObject>
#include <QUrl>
#include <QVariantMap>
#include <QTest>
#include "base_testcase.h"
#include "download.h"

using namespace Ubuntu::Transfers;

class TestBaseDownload : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestBaseDownload(QObject *parent = 0)
        : BaseTestCase("TestBaseDownload", parent) {}

 private slots:  // NOLINT(whitespace/indent)

    void init() override;

    void testStartQueued();
    void testStartNotQueued();
    void testPauseQueued();
    void testPauseNotQueued();
    void testResumeQueued();
    void testResumeNotQueued();
    void testCancelQueued();
    void testCancelNotQueued();

    void testSetHeaders_data();
    void testSetHeaders();

    void testSetHeadersWrongState_data();
    void testSetHeadersWrongState();

 private:
    QString _id;
    QString _appId;
    QString _path;
    bool _isConfined;
    QString _rootPath;
    QUrl _url;
    QVariantMap _metadata;
    QMap<QString, QString> _headers;
};

// register the enum so that it can be used in the data driven tests
Q_DECLARE_METATYPE(Transfer::State)

#endif // TEST_BASE_DOWNLOAD_H

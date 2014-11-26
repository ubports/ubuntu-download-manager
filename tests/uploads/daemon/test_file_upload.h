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

#ifndef TEST_FILE_UPLOAD_H
#define TEST_FILE_UPLOAD_H

#include <QObject>
#include <ubuntu/uploads/file_upload.h>
#include <file_manager.h>
#include <request_factory.h>

#include "uuid_factory.h"
#include "base_testcase.h"

using namespace Ubuntu::Transfers::System;
using namespace Ubuntu::Transfers::Tests;
using namespace Ubuntu::UploadManager;
using namespace Ubuntu::UploadManager::Daemon;

class TestFileUpload : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestFileUpload(QObject *parent = 0)
        : BaseTestCase("TestFileUpload", parent) {}

 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;
    void testIsErrorWhenPathNotAbs();
    void testIsErrorWhenPathMissing();
    void testIsPausable();
    void testStartCannotOpenFile();
    void testStartCorrectHeaders();
    void testSetThrottlePresentReply();
    void testFinishedEmitted();
    void testUploadProgressEmitted();

 private:
    void verifyMocks();

 private:
    QString _id;
    QString _appId;
    QString _path;
    bool _isConfined;
    QString _rootPath;
    QUrl _url;
    QString _filePath;
    QVariantMap _metadata;
    QMap<QString, QString> _headers;
    MockRequestFactory* _reqFactory;
    MockFileManager* _fileManager;
};

#endif

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

#ifndef TEST_DOWNLOAD_H
#define TEST_DOWNLOAD_H

#include <QObject>
#include <base_testcase.h>
#include <gmock/gmock.h>

#include "download.h"
#include "manager.h"
#include "testable_single_download.h"

using namespace Ubuntu::DownloadManager;

MATCHER_P(MetadataEq, value, "Returns if the metadata is equal.") {
    auto argMap = static_cast<QVariantMap>(arg);
    auto valueMap = static_cast<Metadata*>(value);

    bool areEqual = true;

    if (argMap.contains("title")) {
        areEqual &= argMap["title"].toString() == valueMap->title();
    } else {
        areEqual &= valueMap->title().isEmpty();
    }

    if (argMap.contains("indicator-shown")) {
        areEqual &= argMap["indicator-shown"].toBool() == valueMap->showInIndicator();
    }

    return areEqual;
}

class TestSingleDownload : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestSingleDownload(QObject *parent = 0);

    void init() override;
    void cleanup() override;

 private slots:  // NOLINT(whitespace/indent)
    void testCanceledSignalFwd();
    void testFinishedSignalFwd();
    void testPausedSignalFwd();
    void testProcessingSignalFwd();
    void testProgressSignalFwd();
    void testResumedSignalFwd();
    void testStartedSignalFwd();
    void testSetAllowMobileDataNullptr();
    void testSetAllowMobileDataError();
    void testSetAllowMobileDataSuccess();
    void testSetThrottleNullptr();
    void testSetThrottleError();
    void testSetThrottleSuccess();
    void testSetHeadersNullptr();
    void testSetHeadersError();
    void testSetHeadersSuccess();
    void testSetMetadataNullptr();
    void testSetMetadataToNullptr();
    void testSetMetadataError();
    void testSetMetadataSuccess();

 private:
    void verifyMocks();

 private:
    QString _url;
    QVariantMap _metadata;
    QMap<QString, QString> _headers;
    MockDownload* _down;
    MockManager* _man;
};

#endif

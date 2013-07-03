/*
 * Copyright 2013 Canonical Ltd.
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

#ifndef TEST_APP_DOWNLOAD_H
#define TEST_APP_DOWNLOAD_H

#include <QDir>
#include <QObject>
#include <download.h>
#include "fake_request_factory.h"
#include "test_runner.h"

class TestDownload: public QObject
{
    Q_OBJECT
public:
    explicit TestDownload(QObject *parent = 0);

private slots:

    void init();
    void cleanup();

    // constructors tests
    void testNoHashConstructor();
    void testHashConstructor();

    // data function to be used for the accessor tests
    void testNoHashConstructor_data();
    void testHashConstructor_data();
    void testPath_data();
    void testUrl_data();
    void testApplicationId_data();
    void testApplicationName_data();
    void testProgress_data();
    void testMetadata_data();
    void testOnSuccessHash_data();

    // accessor methods
    void testPath();
    void testUrl();
    void testApplicationId();
    void testApplicationName();
    void testMetadata();
    void testProgress();
    void testTotalSize();
    void testTotalSizeNoProgress();

    // dbus method tests
    void testCancel();
    void testPause();
    void testResume();
    void testStart();

    // network related tests
    void testCancelDownload();
    void testCancelDownloadNotStarted();
    void testPauseDownload();
    void testPauseDownloadNotStarted();
    void testResumeRunning();
    void testResumeDownload();
    void testStartDownload();
    void testStartDownloadAlreadyStarted();
    void testOnSuccessNoHash();
    void testOnSuccessHashError();
    void testOnSuccessHash();
    void testOnHttpError();

private:
    bool removeDir(const QString& dirName);

private:
    QDir _testDir;
    QString _appId;
    QString _appName;
    QString _path;
    QUrl _url;
    QCryptographicHash::Algorithm _algo;
    FakeRequestFactory* _reqFactory;

};

DECLARE_TEST(TestDownload)

#endif // TEST_APP_DOWNLOAD_H

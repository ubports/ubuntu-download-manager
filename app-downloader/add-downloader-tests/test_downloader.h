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

#ifndef TEST_DOWNLOADER_H
#define TEST_DOWNLOADER_H

#include <QObject>
#include "test_runner.h"

class TestDownloader : public QObject
{
    Q_OBJECT
public:
    explicit TestDownloader(QObject *parent = 0);
    
private slots:

    void init();
    void cleanup();

    // test exposed via dbus
    void testCreateDownload();
    void testCreateDownloadWithHash();
    void testGetAllDownloads();

    // test q methods
    void testStartNoCurrent();
    void testStartCurrent();
    void testPauseCurrent();
    void testPauseNotCurrent();
    void testResumeCurrent();
    void testResumeNoCurrent();
};

DECLARE_TEST(TestDownloader)

#endif // TEST_DOWNLOADER_H

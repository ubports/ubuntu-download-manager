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

#include "test_downloader.h"

TestDownloader::TestDownloader(QObject *parent) :
    QObject(parent)
{
}

void TestDownloader::init()
{
}

void TestDownloader::cleanup()
{
}

void TestDownloader::testCreateDownload()
{
    QFAIL("Not implemented.");
}

void TestDownloader::testCreateDownloadWithHash()
{
    QFAIL("Not implemented.");
}

void TestDownloader::testGetAllDownloads()
{
    QFAIL("Not implemented.");
}

void TestDownloader::testStartNoCurrent()
{
    QFAIL("Not implemented.");
}

void TestDownloader::testStartCurrent()
{
    QFAIL("Not implemented.");
}

void TestDownloader::testPauseCurrent()
{
    QFAIL("Not implemented.");
}

void TestDownloader::testPauseNotCurrent()
{
    QFAIL("Not implemented.");
}

void TestDownloader::testResumeCurrent()
{
    QFAIL("Not implemented.");
}

void TestDownloader::testResumeNoCurrent()
{
    QFAIL("Not implemented.");
}

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

#ifndef TESTING_FILE_DOWNLOAD_H
#define TESTING_FILE_DOWNLOAD_H

#include <QDBusContext>
#include <QObject>
#include <downloads/file_download.h>

using namespace Ubuntu::DownloadManager::Daemon;

class TestingFileDownload : public FileDownload, public QDBusContext {
    Q_OBJECT

 public:
    TestingFileDownload(FileDownload* down,
                        QObject* parent = 0);
    virtual ~TestingFileDownload();
    void returnDBusErrors(bool errors);

 public slots:  // NOLINT(whitespace/indent)
    qulonglong progress() override;
    qulonglong totalSize() override;
    void setThrottle(qulonglong speed) override;

 private:
    bool _returnErrors;
    FileDownload* _down = nullptr;
};

#endif // TESTING_FILE_DOWNLOAD_H

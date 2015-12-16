/*
 * Copyright 2015 Canonical Ltd.
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

#ifndef DOWNLOAD_HISTORY_H
#define DOWNLOAD_HISTORY_H

#include <ubuntu/download_manager/manager.h>
#include <ubuntu/download_manager/downloads_list.h>

#include "single_download.h"

namespace Ubuntu {

namespace DownloadManager {

class DownloadHistory : public QObject {
    Q_OBJECT

 public:
    explicit DownloadHistory(QObject* parent=0);
    static DownloadHistory *instance();

    QVariantList downloads() const;
    void downloadsFound(DownloadsList* downloadsList);
    void addDownload(SingleDownload *singleDownload);
    bool cleanDownloads() const;
    void setCleanDownloads(bool value);

 signals:
    void downloadsChanged();
    void downloadFinished(SingleDownload *singleDownload, const QString& path);
    void errorFound(SingleDownload *download);
    void downloadPaused(SingleDownload *singleDownload);
    void downloadResumed(SingleDownload *singleDownload);
    void downloadCanceled(SingleDownload *singleDownload);

 private slots:
    void downloadCompleted(const QString& path);
    void onError(DownloadError& error);
    void onPaused();
    void onResumed();
    void onCanceled();

 private:
    Manager* m_manager;
    QVariantList m_downloads;
    bool m_cleanDownloads;
};

} // Ubuntu

} // DownloadManager

#endif

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

#ifndef UBUNTUDOWNLOADMANAGER_PLUGIN_H
#define UBUNTUDOWNLOADMANAGER_PLUGIN_H

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include <ubuntu/download_manager/manager.h>

#include "download_error.h"
#include "single_download.h"

namespace Ubuntu {

namespace DownloadManager {

class UbuntuDownloadManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoStart READ autoStart WRITE setAutoStart)
    Q_PROPERTY(bool cleanDownloads READ cleanDownloads WRITE setCleanDownloads)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorChanged)
    Q_PROPERTY(QVariantList downloads READ downloads NOTIFY downloadsChanged)

public:
    explicit UbuntuDownloadManager(QObject *parent = 0);
    ~UbuntuDownloadManager();

    Q_INVOKABLE void download(QString url);

    QVariantList downloads();
    QString errorMessage() const { return m_errorMessage; }
    bool autoStart() const { return m_autoStart; }
    bool cleanDownloads() const;
    void setCleanDownloads(bool value);
    void setAutoStart(bool value) { m_autoStart = value; }

signals:
    void errorChanged();
    void downloadsChanged();
    void downloadFinished(SingleDownload *download, const QString& path);
    void errorFound(SingleDownload *download);
    void downloadPaused(SingleDownload *download);
    void downloadResumed(SingleDownload *download);
    void downloadCanceled(SingleDownload *download);

private slots:
    void registerError(DownloadError& error);
    void downloadFileCreated(Download* download);
    void downloadGroupCreated(GroupDownload* group);
    void downloadsFound(DownloadsList* downloads);

private:
    bool m_autoStart;
    bool m_cleanDownloads;
    QString m_errorMessage;
    Manager* m_manager;
};

}
}

#endif // UBUNTUDOWNLOADMANAGER_PLUGIN_H

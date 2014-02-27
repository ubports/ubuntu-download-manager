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
    Q_PROPERTY(bool cleanDownloads READ cleanDownloads WRITE setCleanDownloads)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorChanged)
    Q_PROPERTY(QVariantList downloads READ downloads NOTIFY downloadsChanged)

public:
    explicit UbuntuDownloadManager(QObject *parent = 0);
    ~UbuntuDownloadManager();

    Q_INVOKABLE void download(QString url);

    QVariantList downloads() { return m_downloads; }
    QString errorMessage() { return m_errorMessage; }
    bool cleanDownloads() { return m_cleanDownloads; }
    void setCleanDownloads(bool value);

signals:
    void errorChanged();
    void downloadsChanged();

private slots:
    void registerError(DownloadError& error);
    void downloadFileCreated(Download* download);
    void downloadGroupCreated(GroupDownload* group);
    void downloadCompleted();

private:
    bool m_cleanDownloads;
    QVariantList m_downloads;
    QString m_errorMessage;
    Manager* m_manager;
};

}
}

#endif // UBUNTUDOWNLOADMANAGER_PLUGIN_H

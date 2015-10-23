#include "download_history.h"
#include <glog/logging.h>
#include <ubuntu/download_manager/download_struct.h>
#include <QCoreApplication>

namespace Ubuntu {

namespace DownloadManager {

DownloadHistory::DownloadHistory(QObject *parent) :
    QObject(parent),
    m_manager(nullptr)
{
    m_manager = Manager::createSessionManager("", this);

    CHECK(connect(m_manager, &Manager::downloadsFound,
        this, &DownloadHistory::downloadsFound))
            << "Could not connect to signal";

    // Get previous downloads for this app
    auto environment = QProcessEnvironment::systemEnvironment();
    if (environment.contains("APP_ID")) {
        m_manager->getUncollectedDownloads(environment.value("APP_ID"));
    } else {
        m_manager->getUncollectedDownloads(QCoreApplication::applicationFilePath());
    }
}

DownloadHistory *DownloadHistory::instance()
{
    static DownloadHistory *downloadHistory = new DownloadHistory();
    return downloadHistory;
}

QVariantList DownloadHistory::downloads() const
{
    return m_downloads;
}

void DownloadHistory::addDownload(SingleDownload *singleDownload)
{
    m_downloads.append(QVariant::fromValue(singleDownload));
    emit downloadsChanged();
}

void DownloadHistory::downloadsFound(DownloadsList* downloadsList)
{
    foreach(QSharedPointer<Download> download, downloadsList->downloads()) {
        SingleDownload* singleDownload = new SingleDownload(this);
        CHECK(connect(singleDownload, &SingleDownload::finished,
            this, &DownloadHistory::downloadCompleted))
                << "Could not connect to signal";
        singleDownload->bindDownload(download.data());
        m_downloads.append(QVariant::fromValue(singleDownload));
    }
    emit downloadsChanged();
}

bool DownloadHistory::cleanDownloads() const
{
    return m_cleanDownloads;
}

void DownloadHistory::setCleanDownloads(bool value)
{
    m_cleanDownloads = value;
    if (m_cleanDownloads) {
        QVariantList newList;
        foreach(QVariant var, m_downloads) {
            SingleDownload *download = qobject_cast<SingleDownload*>(var.value<SingleDownload*>());
            if (download != nullptr && !download->isCompleted()) {
                newList.append(QVariant::fromValue(download));
            } else {
                download->deleteLater();
            }
        }
        m_downloads = newList;
        emit downloadsChanged();
    }
}

void DownloadHistory::downloadCompleted(const QString& path)
{
    SingleDownload* download = qobject_cast<SingleDownload*>(sender());
    if (download != nullptr) {
        emit downloadFinished(download, path);
        if (m_cleanDownloads) {
            int index = m_downloads.indexOf(QVariant::fromValue(download));
            m_downloads.removeAt(index);
            emit downloadsChanged();
        }
    }
}

}
}

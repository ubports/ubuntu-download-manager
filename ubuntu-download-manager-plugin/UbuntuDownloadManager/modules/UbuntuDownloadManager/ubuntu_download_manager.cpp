#include "ubuntu_download_manager.h"
#include <ubuntu/download_manager/download_struct.h>

namespace Ubuntu {

namespace DownloadManager {

UbuntuDownloadManager::UbuntuDownloadManager(QObject *parent) :
    QObject(parent),
    m_errorMessage(""),
    m_manager(nullptr)
{
    m_manager = Manager::createSessionManager("", this);

    QObject::connect(m_manager, SIGNAL(downloadCreated(Download*)),
                     this, SLOT(downloadFileCreated(Download*)));
}

UbuntuDownloadManager::~UbuntuDownloadManager()
{
    if (m_manager != nullptr) {
        m_manager->deleteLater();
    }
    m_downloads.clear();
}

void UbuntuDownloadManager::download(QString url)
{
    DownloadStruct dstruct(url);
    m_manager->createDownload(dstruct);
}

void UbuntuDownloadManager::downloadFileCreated(Download* download)
{
    SingleDownload* singleDownload = new SingleDownload(this);
    QObject::connect(singleDownload, SIGNAL(errorFound(DownloadError&)),
                     this, SLOT(registerError(DownloadError&)));
    QObject::connect(singleDownload, SIGNAL(finished(const QString& path)),
                     this, SLOT(downloadCompleted()));
    singleDownload->bindDownload(download);
    m_downloads.append(QVariant::fromValue(singleDownload));
    emit downloadsChanged();
    singleDownload->start();
}

void UbuntuDownloadManager::downloadGroupCreated(GroupDownload* group)
{
    Q_UNUSED(group);
}

void UbuntuDownloadManager::registerError(DownloadError& error)
{
    m_errorMessage = error.message();
    emit errorChanged();
}

void UbuntuDownloadManager::setCleanDownloads(bool value)
{
    m_cleanDownloads = value;
    if (m_cleanDownloads) {
        QVariantList newList;
        for (int i = 0; i < m_downloads.size(); ++i) {
            SingleDownload *download = qobject_cast<SingleDownload*>(m_downloads.at(i).value<SingleDownload*>());
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

void UbuntuDownloadManager::downloadCompleted()
{
    if (m_cleanDownloads) {
        SingleDownload* download = qobject_cast<SingleDownload*>(sender());
        if (download != nullptr) {
            int index = m_downloads.indexOf(QVariant::fromValue(download));
            m_downloads.removeAt(index);
            emit downloadsChanged();
            download->deleteLater();
        }
    }
}

}
}

#include "ubuntu_download_manager.h"
#include <glog/logging.h>
#include <ubuntu/download_manager/download_struct.h>
#include <QDebug>

namespace Ubuntu {

namespace DownloadManager {

/*!
    \qmltype DownloadManager
    \instantiates DownloadManager
    \inqmlmodule Ubuntu.DownloadManager 0.1
    \ingroup download
    \brief Manage downloads for several files.

    DownloadManager provides facilities for downloading a several
    files, connect the downloads property to any Item that works
    with models, and dynamically update the content of those
    lists/repeaters/etc to show the current downloads and connect
    any UI to the SingleDownload properties in the delegates.

    Example usage:

    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1
    import Ubuntu.DownloadManager 0.1

    Rectangle {
        width: units.gu(100)
        height: units.gu(80)

        DownloadManager {
            id: manager
        }

        TextField {
            id: text
            placeholderText: "File URL to download..."
            height: 50
            anchors {
                left: parent.left
                right: button.left
                rightMargin: units.gu(2)
            }
        }

        Button {
            id: button
            text: "Download"
            height: 50
            anchors.right: parent.right

            onClicked: {
                manager.download(text.text);
            }
        }

        ListView {
            id: list
            anchors {
                left: parent.left
                right: parent.right
                top: text.bottom
                bottom: parent.bottom
            }
            model: manager.downloads

            delegate: ProgressBar {
                minimumValue: 0
                maximumValue: 100
                value: modelData.progress
            }
        }
    }
    \endqml

    \sa {SingleDownload}
*/

UbuntuDownloadManager::UbuntuDownloadManager(QObject *parent) :
    QObject(parent),
    m_autoStart(true),
    m_errorMessage(""),
    m_manager(nullptr)
{
    m_manager = Manager::createSessionManager("", this);

    // Get previous downloads for this app
    m_manager->getAllDownloads();

    CHECK(connect(m_manager, &Manager::downloadCreated,
        this, &UbuntuDownloadManager::downloadFileCreated))
            << "Could not connect to signal";
    CHECK(connect(m_manager, &Manager::downloadsFound,
        this, &UbuntuDownloadManager::downloadsFound))
            << "Could not connect to signal";
}

UbuntuDownloadManager::~UbuntuDownloadManager()
{
    if (m_manager != nullptr) {
        m_manager->deleteLater();
    }
    m_downloads.clear();
}

/*!
    \qmlmethod void DownloadManager::download(string url)

    Starts the download for the given url and update the model with the list of the different downloads.
*/
void UbuntuDownloadManager::download(QString url)
{
    DownloadStruct dstruct(url);
    m_manager->createDownload(dstruct);
}

void UbuntuDownloadManager::downloadFileCreated(Download* download)
{
    SingleDownload* singleDownload = new SingleDownload(this);
    CHECK(connect(singleDownload, &SingleDownload::errorFound,
        this, &UbuntuDownloadManager::registerError))
            << "Could not connect to signal";
    CHECK(connect(singleDownload, &SingleDownload::finished,
        this, &UbuntuDownloadManager::downloadCompleted))
            << "Could not connect to signal";
    singleDownload->bindDownload(download);
    m_downloads.append(QVariant::fromValue(singleDownload));
    emit downloadsChanged();
    if (m_autoStart) {
        singleDownload->startDownload();
    }
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

void UbuntuDownloadManager::downloadsFound(DownloadsList* downloadsList)
{
    foreach(QSharedPointer<Download> download, downloadsList->downloads()) {
        SingleDownload* singleDownload = new SingleDownload(this);
        CHECK(connect(singleDownload, &SingleDownload::errorFound,
            this, &UbuntuDownloadManager::registerError))
                << "Could not connect to signal";
        CHECK(connect(singleDownload, &SingleDownload::finished,
            this, &UbuntuDownloadManager::downloadCompleted))
                << "Could not connect to signal";
        singleDownload->bindDownload(download.data());
        m_downloads.append(QVariant::fromValue(singleDownload));
    }
    emit downloadsChanged();
}

/*!
    \qmlproperty bool DownloadManager::autoStart

    This property sets if the downloads should start automatically, or let the user
    decide when to start them calling the "start()" method on each download.
*/

/*!
    \qmlproperty bool DownloadManager::cleanDownloads

    This property sets if the model with the list of downloads should be cleaned
    when a download finish. Removing those completed transfers, and only showing
    current downloads.
*/

/*!
    \qmlproperty string DownloadManager::errorMessage

    The error message associated with the current download, if there is any.
*/

/*!
    \qmlproperty list DownloadManager::downloads

    This property provides a model that can be used for Lists, Repeaters, etc. To handle
    those downloads in a UI component that changes automatically when a new download is
    started, and let a custom delegate how to represent the UI for each download.
*/

}
}

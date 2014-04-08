#include "single_download.h"
#include <glog/logging.h>
#include <ubuntu/download_manager/download_struct.h>
#include <QDebug>

namespace Ubuntu {

namespace DownloadManager {

/*!
    \qmltype SingleDownload
    \instantiates SingleDownload
    \inqmlmodule UbuntuDownloadManager 0.1
    \ingroup download
    \brief Manage file downloads and tracking the progress.

    SingleDownload provides facilities for downloading a single
    file, track the process, react to error conditions, etc.

    Example usage:

    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1
    import UbuntuDownloadManager 0.1

    Rectangle {
        width: units.gu(100)
        height: units.gu(20)

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
                single.download(text.text);
            }
        }

        ProgressBar {
            minimumValue: 0
            maximumValue: 100
            value: single.progress
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            SingleDownload {
                id: single
            }
        }
    }
    \endqml

    \sa {DownloadManager}
*/

SingleDownload::SingleDownload(QObject *parent) :
    QObject(parent),
    m_autoStart(true),
    m_completed(false),
    m_downloading(false),
    m_downloadInProgress(false),
    m_progress(0),
    m_error(this),
    m_download(nullptr),
    m_manager(nullptr)
{
}

void SingleDownload::bindDownload(Download* download)
{
    m_download = download;

    CHECK(connect(m_download,
        static_cast<void(Download::*)(Error*)>(&Download::error),
        this, &SingleDownload::registerError))
            << "Could not connect to signal";
    CHECK(connect(m_download, &Download::finished, this,
        &SingleDownload::finished)) << "Could not connect to signal";
    CHECK(connect(m_download, &Download::finished, this,
        &SingleDownload::setCompleted)) << "Could not connect to signal";
    CHECK(connect(m_download,
         static_cast<void(Download::*)(qulonglong, qulonglong)>(
             &Download::progress), this,
         &SingleDownload::progressReceived))
            << "Could not connect to signal";
    CHECK(connect(m_download,
         static_cast<void(Download::*)(qulonglong, qulonglong)>(
             &Download::progress), this,
         &SingleDownload::setProgress)) << "Could not connect to signal";
    CHECK(connect(m_download, &Download::canceled, this,
         &SingleDownload::canceled)) << "Could not connect to signal";
    CHECK(connect(m_download, &Download::canceled, this,
         &SingleDownload::setDownloadCanceled))
            << "Could not connect to signal";
    CHECK(connect(m_download, &Download::paused, this,
         &SingleDownload::paused)) << "Could not connect to signal";
    CHECK(connect(m_download, &Download::paused, this,
         &SingleDownload::setDownloadPaused))
            << "Could not connect to signal";
    CHECK(connect(m_download, &Download::resumed, this,
         &SingleDownload::resumed)) << "Could not connect to signal";
    CHECK(connect(m_download, &Download::resumed, this,
         &SingleDownload::setDownloadStarted))
            << "Could not connect to signal";
    CHECK(connect(m_download, &Download::started, this,
         &SingleDownload::started)) << "Could not connect to signal";
    CHECK(connect(m_download, &Download::started, this,
         &SingleDownload::setDownloadStarted))
            << "Could not connect to signal";

    if (m_manager != nullptr && m_autoStart) {
        startDownload();
    }
}

/*!
    \qmlmethod void SingleDownload::download(string url)

    Creates the download for the given url and reports the different states through the properties.
*/
void SingleDownload::download(QString url)
{
    if (!m_downloadInProgress) {
        if (m_manager == nullptr) {
            m_manager = Manager::createSessionManager("", this);

            CHECK(connect(m_manager, &Manager::downloadCreated, this,
                &SingleDownload::bindDownload))
                    << "Could not connect to signal";
        }
        DownloadStruct dstruct(url);
        m_manager->createDownload(dstruct);
    } else {
        m_error.setMessage("Current download still in progress.");
        emit errorChanged();
    }
}

/*!
    \qmlmethod void SingleDownload::start()

    Starts the download, used when autoStart is False.
*/
void SingleDownload::start()
{
    startDownload();
}

void SingleDownload::startDownload()
{
    if (m_download != nullptr) {
        m_download->start();
    }
}

void SingleDownload::pause()
{
    m_download->pause();
}

void SingleDownload::resume()
{
    m_download->resume();
}

void SingleDownload::cancel()
{
    m_download->cancel();
}

void SingleDownload::setCompleted()
{
    m_completed = true;
    m_downloading = false;
    m_downloadInProgress = false;
}

void SingleDownload::registerError(Error* error)
{
    m_error.setMessage(error->errorString());
    emit errorFound(m_error);
    emit errorChanged();
}

void SingleDownload::setProgress(qulonglong received, qulonglong total)
{
    if (total > 0) {
        qulonglong result = (received * 100);
        m_progress = static_cast<int>(result / total);
        emit progressChanged();
    }
}

void SingleDownload::setDownloadPaused(bool)
{
    m_downloading = false;
}

void SingleDownload::setDownloadStarted(bool)
{
    m_downloading = true;
}

void SingleDownload::setDownloadCanceled(bool)
{
    m_completed = false;
    m_downloading = false;
    m_downloadInProgress = false;
}

/*!
    \qmlproperty bool SingleDownload::autoStart

    This property sets if the downloads should start automatically, or let the user
    decide when to start them calling the "start()" method.
*/

/*!
    \qmlproperty string SingleDownload::errorMessage

    The error message associated with the current download, if there is any.
*/

/*!
    \qmlproperty bool SingleDownload::isCompleted

    The current state of the download. True if the download already finished, False otherwise.
*/

/*!
    \qmlproperty bool SingleDownload::downloadInProgress

    This property represents if the download is active, no matter if it's paused or anything.
    If a download is active, the value will be True. It will become False when the download
    finished or get canceled.
*/

/*!
    \qmlproperty bool SingleDownload::allowMobileDownload

    This property sets if the download handled by this object will work under mobile data connection.
*/

/*!
    \qmlproperty long SingleDownload::throttle

    This property can be used to limit the bandwidth used for the download.
*/

/*!
    \qmlproperty int SingleDownload::progress

    This property reports the current progress in percentage of the download, from 0 to 100.
*/

/*!
    \qmlproperty bool SingleDownload::downloading

    This property represents the current state of the download.
    False if paused or not downloading anything.
    True if the file is currently being downloaded.
*/

}
}

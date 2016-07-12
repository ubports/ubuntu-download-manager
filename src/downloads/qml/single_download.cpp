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

#include <glog/logging.h>
#include <ubuntu/download_manager/download_struct.h>

#include "single_download.h"
#include "download_history.h"

namespace Ubuntu {

namespace DownloadManager {

/*!
    \qmltype SingleDownload
    \instantiates SingleDownload
    \inqmlmodule Ubuntu.DownloadManager 1.2
    \ingroup download
    \brief Manage file downloads and tracking the progress.

    SingleDownload provides facilities for downloading a single
    file, track the process, react to error conditions, etc.

    Example usage:

    \qml
    import QtQuick 2.0
    import Ubuntu.Components 1.2
    import Ubuntu.DownloadManager 1.2

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

SingleDownload::SingleDownload(QObject *parent)
    : QObject(parent),
      m_autoStart(true),
      m_completed(false),
      m_downloading(false),
      m_downloadInProgress(false),
      m_progress(0),
      m_error(this),
      m_download(nullptr),
      m_manager(nullptr) {
}

// constructor that is simple provided for testing purposes
SingleDownload::SingleDownload(Download* down, Manager* man, QObject *parent)
    : QObject(parent),
      m_autoStart(false),
      m_completed(false),
      m_downloading(false),
      m_downloadInProgress(false),
      m_progress(0),
      m_error(this),
      m_download(down),
      m_manager(man) {
    if (m_download != nullptr)
        bindDownload(m_download);
}

void
SingleDownload::bindDownload(Download* download)
{
    m_download = download;

    CHECK(connect(m_download,
        static_cast<void(Download::*)(Error*)>(&Download::error),
        this, &SingleDownload::registerError))
            << "Could not connect to signal";

    CHECK(connect(m_download, &Download::finished, this,
        &SingleDownload::onFinished)) << "Could not connect to signal";

    CHECK(connect(m_download,
         static_cast<void(Download::*)(qulonglong, qulonglong)>(
             &Download::progress), this,
         &SingleDownload::onProgress)) << "Could not connect to signal";

    CHECK(connect(m_download, &Download::canceled, this,
         &SingleDownload::onCanceled))
            << "Could not connect to signal";

    CHECK(connect(m_download, &Download::paused, this,
         &SingleDownload::onPaused))
            << "Could not connect to signal";

    CHECK(connect(m_download, &Download::processing, this,
        &SingleDownload::processing)) << "Could not connect to signal";

    CHECK(connect(m_download, &Download::resumed, this,
         &SingleDownload::onResumed))
            << "Could not connect to signal";

    CHECK(connect(m_download, &Download::started, this,
         &SingleDownload::onStarted))
            << "Could not connect to signal";

    // is the current in memory setting dirty, if they are, we do set the before we
    // start
    if (m_dirty) {
        setAllowMobileDownload(m_mobile);
        setThrottle(m_throttle);
        setHeaders(m_headers);
        setMetadata(m_metadata);

        // is not really needed but we do it to be consistent
        m_dirty = false;
    }

    if (m_manager != nullptr && m_autoStart) {
        startDownload();
    }

    DownloadHistory::instance()->addDownload(this);

    // Keep a record of the downloadId so clients can still access the property
    // after a download has finished
    m_downloadId = m_download->id();

    emit downloadIdChanged();
}

void
SingleDownload::unbindDownload(Download* download) {
    CHECK(disconnect(download,
        static_cast<void(Download::*)(Error*)>(&Download::error),
        this, &SingleDownload::registerError))
            << "Could not connect to signal";

    CHECK(disconnect(download, &Download::finished, this,
        &SingleDownload::onFinished)) << "Could not connect to signal";

    CHECK(disconnect(download,
         static_cast<void(Download::*)(qulonglong, qulonglong)>(
             &Download::progress), this,
         &SingleDownload::onProgress)) << "Could not connect to signal";

    CHECK(disconnect(download, &Download::canceled, this,
         &SingleDownload::onCanceled))
            << "Could not connect to signal";

    CHECK(disconnect(download, &Download::paused, this,
         &SingleDownload::onPaused))
            << "Could not connect to signal";

    CHECK(disconnect(m_download, &Download::processing, this,
        &SingleDownload::processing)) << "Could not connect to signal";

    CHECK(disconnect(download, &Download::resumed, this,
         &SingleDownload::onResumed))
            << "Could not connect to signal";

    CHECK(disconnect(download, &Download::started, this,
         &SingleDownload::onStarted))
            << "Could not connect to signal";
}

/*!
    \qmlmethod void SingleDownload::download(string url)

    Creates the download for the given url and reports the different states through the properties.
*/
void
SingleDownload::download(QString url)
{
    if (!m_downloadInProgress && !url.isEmpty()) {
        if (m_manager == nullptr) {
            m_manager = Manager::createSessionManager("", this);

            CHECK(connect(m_manager, &Manager::downloadCreated, this,
                &SingleDownload::bindDownload))
                    << "Could not connect to signal";
        }
        Metadata metadata;
        QMap<QString, QString> headers;
        DownloadStruct dstruct(url, m_hash, m_algorithm, metadata.map(), headers);
        m_manager->createDownload(dstruct);
    } else if (url.isEmpty()) {
        m_error.setMessage("No URL specified");
        emit errorChanged();
    } else {
        m_error.setMessage("Current download still in progress.");
        emit errorChanged();
    }
}

/*!
    \qmlmethod void SingleDownload::start()

    Starts the download, used when autoStart is False.
*/
void
SingleDownload::start()
{
    startDownload();
}

void
SingleDownload::startDownload()
{
    if (m_download != nullptr) {
        m_download->start();
    }
}

/*!
    \qmlmethod void SingleDownload::pause()

    Pauses the download. An error is returned if the download was
    already paused.
*/
void
SingleDownload::pause()
{
    if (m_download != nullptr) {
        m_download->pause();
    }
}

/*!
    \qmlmethod void SingleDownload::resume()

    Resumes and already paused download. An error is returned if the download was
    already resumed or not paused.
*/
void
SingleDownload::resume()
{
    if (m_download != nullptr) {
        m_download->resume();
    }
}

/*!
    \qmlmethod void SingleDownload::cancel()

    Cancels a download.
*/
void
SingleDownload::cancel()
{
    if (m_download != nullptr) {
        m_download->cancel();
    }
}

void
SingleDownload::registerError(Error* error)
{
    m_error.setMessage(error->errorString());
    m_download = nullptr;
    emit errorFound(m_error);
    emit errorChanged();
}

void
SingleDownload::onFinished(const QString& path)
{
    m_completed = true;
    m_downloading = false;
    m_downloadInProgress = false;

    // unbind the download so that we have no memory leaks due to the connections
    unbindDownload(m_download);
    emit finished(path);
}

void
SingleDownload::onProgress(qulonglong received, qulonglong total)
{
    if (total > 0) {
        qulonglong result = (received * 100);
        m_progress = static_cast<int>(result / total);
        emit progressChanged();
    }
    emit progressReceived(received, total);
}

void
SingleDownload::onPaused(bool wasPaused)
{
    m_downloading = false;
    emit paused(wasPaused);
}

void
SingleDownload::onResumed(bool wasResumed) {
    m_downloading = true;
    emit resumed(wasResumed);
}

void
SingleDownload::onStarted(bool wasStarted)
{
    m_downloading = true;
    emit started(wasStarted);
}

void
SingleDownload::onCanceled(bool wasCanceled)
{
    m_completed = false;
    m_downloading = false;
    m_downloadInProgress = false;

    // unbind the download so that we have no memory leaks due to the connections
    unbindDownload(m_download);
    emit canceled(wasCanceled);
}

bool
SingleDownload::allowMobileDownload() const {
    if (m_download == nullptr) {
        return m_mobile;
    } else {
        return m_download->isMobileDownloadAllowed();
    }
}

void
SingleDownload::setAllowMobileDownload(bool value) {
    if (m_download == nullptr) {
        m_dirty = true;
        m_mobile = value;
    } else {
        m_download->allowMobileDownload(value);
        if (m_download->isError()) {
            // set the error details and emit the signals
            auto err = m_download->error();
            m_error.setType(getErrorType(err->type()));
            m_error.setMessage(err->errorString());
            emit errorFound(m_error);
            emit errorChanged();
        } else {
            emit allowMobileDownloadChanged();
        }
    }
}

qulonglong
SingleDownload::throttle() const {
    if (m_download == nullptr) {
        return m_throttle;
    } else {
        return m_download->throttle();
    }
}

void
SingleDownload::setThrottle(qulonglong value) {
    if (m_download == nullptr) {
        m_dirty = true;
        m_throttle = value;
    } else {
        m_download->setThrottle(value);
        if (m_download->isError()) {
            // set the error details and emit the signals
            auto err = m_download->error();
            m_error.setType(getErrorType(err->type()));
            m_error.setMessage(err->errorString());
            emit errorFound(m_error);
            emit errorChanged();
        } else {
            emit throttleChanged();
        }
    }
}

QString
SingleDownload::downloadId() const {
    if (m_download == nullptr) {
        return m_downloadId;
    } else {
        return m_download->id();
    }
}

QVariantMap
SingleDownload::headers() const {
    if (m_download == nullptr) {
        return m_headers;
    } else {
        // convert the QMap<QString, QString> into a QMap<QString, QVariant>
        auto headers = m_download->headers();
        QVariantMap result;
        foreach(const QString& key, headers.keys()) {
            result[key] = headers[key]; // automatic conversion
        }
        return result;
    }
}

Metadata*
SingleDownload::metadata() const {
    if (m_download == nullptr) {
        return m_metadata;
    } else {
        // convert the QMap<QString, QString> into a QMap<QString, QVariant>
        auto metadata = m_download->metadata();
        auto result = new Metadata(metadata);
        return result;
    }
}

QString
SingleDownload::hash() const {
    return m_hash;
}

QString
SingleDownload::algorithm() const {
    return m_algorithm;
}

void
SingleDownload::setHeaders(QVariantMap headers) {
    if (m_download == nullptr) {
        m_dirty = true;
        m_headers = headers;
    } else {
        QMap<QString, QString> stringMap;
        // convert the QVariantMap in a QMap<QString, QString> and make sure
        // that the variants can be converted to strings.
        foreach(const QString& key, headers.keys()) {
            auto data = headers[key];
            if (data.canConvert<QString>()) {
                stringMap[key] = data.toString();
            } else {
                m_error.setType("Headers Conversion Error");
                auto msg = QString(
                    "Could not convert data in header '%1' to string.").arg(key);
                m_error.setMessage(msg);
                emit errorFound(m_error);
                emit errorChanged();
                return;
            }
        }

        m_download->setHeaders(stringMap);
        if (m_download->isError()) {
            // set the error details and emit the signals
            auto err = m_download->error();
            m_error.setType(getErrorType(err->type()));
            m_error.setMessage(err->errorString());
            emit errorFound(m_error);
            emit errorChanged();
        } else {
            emit headersChanged();
        }
    }
}

void
SingleDownload::setMetadata(Metadata* metadata) {
    if (metadata == nullptr) {
        m_metadata = nullptr;
        return;
    }

    m_metadata = metadata;
    if (m_download == nullptr) {
        m_dirty = true;
    } else {
        m_download->setMetadata(metadata->map());
        if (m_download->isError()) {
            // set the error details and emit the signals
            auto err = m_download->error();
            m_error.setType(getErrorType(err->type()));
            m_error.setMessage(err->errorString());
            emit errorFound(m_error);
            emit errorChanged();
        } else {
            emit metadataChanged();
        }
    }
}

void
SingleDownload::setHash(QString hash) {
    m_hash = hash;
}

void
SingleDownload::setAlgorithm(QString algorithm) {
    m_algorithm = algorithm;
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

/*!
    \qmlproperty string SingleDownload::downloadId

    This property provides the unique identifier that represents the download
    within the download manager.
*/

/*!
    \qmlproperty QVariantMap SingleDownload::headers

    This property allows to get and set the headers that will be used to perform
    the download request. All headers must be strings or at least QVariant should
    be able to convert them to strings.
*/

/*!
    \qmlproperty Metadata SingleDownload::metadata

    This property allows to get and set the metadata that will be linked to 
    the download request. 
*/

/*!
    \qmlproperty string SingleDownload::hash
    \since Ubuntu.DownloadManager 1.3

    This property specifies a hash to check against the downloaded file.
    If used, this should be set prior to calling the download() method.
*/

/*!
    \qmlproperty string SingleDownload::algorithm
    \since Ubuntu.DownloadManager 1.3

    This property indicates the algorithm to use when verifying a hash.
    The algorithm can be of one of the following string values:

         - "md5"
         - "sha1"
         - "sha224"
         - "sha256"
         - "sha384"
         - "sha512"

    If no value is specified md5 will be used.
    If used, this should be set prior to calling the download() method.
*/

/*!
    \qmlsignal SingleDownload::finished(QString path)

    This signal is emitted when a download has finished. The downloaded file 
    path is provided via the 'path' paremeter. The corresponding handler is 
    \c onFinished
*/

}
}

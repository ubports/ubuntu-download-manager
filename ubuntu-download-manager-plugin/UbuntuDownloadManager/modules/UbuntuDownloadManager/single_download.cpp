#include "single_download.h"
#include <ubuntu/download_manager/download_struct.h>
#include <QDebug>

namespace Ubuntu {

namespace DownloadManager {

SingleDownload::SingleDownload(QObject *parent) :
    QObject(parent),
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

    connect(m_download, SIGNAL(error(Error*)), this,
            SLOT(registerError(Error*)));
    connect(m_download, SIGNAL(finished(const QString &)), this,
            SIGNAL(finished(const QString &)));
    connect(m_download, SIGNAL(finished(const QString &)), this,
            SLOT(setCompleted()));
    connect(m_download, SIGNAL(progress(qulonglong, qulonglong)), this,
            SIGNAL(progressReceived(qulonglong, qulonglong)));
    connect(m_download, SIGNAL(progress(qulonglong, qulonglong)), this,
            SLOT(setProgress(qulonglong, qulonglong)));
    connect(m_download, SIGNAL(canceled(bool)), this, SIGNAL(canceled(bool)));
    connect(m_download, SIGNAL(canceled(bool)), this, SLOT(setDownloadCanceled(bool)));
    connect(m_download, SIGNAL(paused(bool)), this, SIGNAL(paused(bool)));
    connect(m_download, SIGNAL(paused(bool)), this, SLOT(setDownloadPaused(bool)));
    connect(m_download, SIGNAL(resumed(bool)), this, SIGNAL(resumed(bool)));
    connect(m_download, SIGNAL(resumed(bool)), this, SLOT(setDownloadStarted(bool)));
    connect(m_download, SIGNAL(started(bool)), this, SIGNAL(started(bool)));
    connect(m_download, SIGNAL(started(bool)), this, SLOT(setDownloadStarted(bool)));

    if (m_manager != nullptr) {
        start();
    }
}

void SingleDownload::download(QString url)
{
    if (!m_downloadInProgress) {
        if (m_manager == nullptr) {
            m_manager = Manager::createSessionManager("", this);

            QObject::connect(m_manager, SIGNAL(downloadCreated(Download*)),
                             this, SLOT(bindDownload(Download*)));
        }
        DownloadStruct dstruct(url);
        m_manager->createDownload(dstruct);
    } else {
        m_error.setMessage("Current download still in progress.");
        emit errorChanged();
    }
}

void SingleDownload::start()
{
    m_download->start();
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

}
}

/*
 * Copyright 2013 Canonical Ltd.
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

#include <QBuffer>
#include <QDir>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSignalMapper>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include "network_reply.h"
#include "xdg_basedir.h"
#include "download.h"

// metadata keys definitions
#define ID "id"
#define NAME "name"
#define SIZE "size"
#define PROGRESS "progress"
#define STATE "state"
#define DBUS_PATH "dbus_path"
#define URL "url"
#define HASH "hash"
#define ALGO "algo"
#define DATA_FILE_NAME "data"
#define METADATA_FILE_NAME "metadata"
#define METADATA_COMMAND_KEY "post-download-command"
#define METADATA_COMMAND_FILE_KEY "$file"


/**
 * PRIVATE IMPLEMENATION
 */

class DownloadPrivate
{
    Q_DECLARE_PUBLIC(Download)
public:
    explicit DownloadPrivate(const QUuid& id, const QString& path, const QUrl& url, const QVariantMap& metadata,
        const QMap<QString, QString>& headers, SystemNetworkInfo* networkInfo, RequestFactory* nam, ProcessFactory* processFactory,
        Download* parent);
    explicit DownloadPrivate(const QUuid& id, const QString& path, const QUrl& url, const QString& hash,
        QCryptographicHash::Algorithm algo, const QVariantMap& metadata, const QMap<QString, QString>& headers,
        SystemNetworkInfo* networkInfo, RequestFactory* nam, ProcessFactory* processFactory,
        Download* parent);
    ~DownloadPrivate();

    // public methods
    QUuid downloadId() const;
    QString path() const;
    QUrl url() const;
    Download::State state();
    void setState(Download::State state);
    QString filePath();
    QString hash() const;
    QCryptographicHash::Algorithm hashAlgorithm() const;
    QMap<QString, QString> headers() const;
    bool canDownload();

    // methods that do really perform the actions
    void cancelDownload();
    void pauseDownload();
    void resumeDownload();
    void startDownload();
    static DownloadPrivate* fromMetadata(const QString &path, RequestFactory* nam, Download* parent);

    // plublic slots used by public implementation
    QVariantMap metadata();
    qulonglong progress();
    qulonglong totalSize();
    void setThrottle(qulonglong speed);
    qulonglong throttle();
    void allowGSMDownload(bool allowed);
    bool isGSMDownloadAllowed();
    void cancel();
    void pause();
    void resume();
    void start();

    // slots executed to keep track of the newtork reply
    void onDownloadProgress(qint64 progress, qint64 bytesTotal);
    void onError(QNetworkReply::NetworkError code);
    void onFinished();
    void onSslErrors(const QList<QSslError>& errors);

    // slots executed to keep track of the post download process
    void onProcessError(QProcess::ProcessError error);
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void init();
    void connectToReplySignals();
    void disconnectFromReplySignals();
    QString saveFileName();
    QString saveMetadataName();
    void storeMetadata();
    bool removeDir(const QString& dirName);
    void cleanUpCurrentData();
    QNetworkRequest buildRequest();

private:
    QUuid _id;
    qulonglong _totalSize;
    qulonglong _throttle;
    bool _allowGSMDownload;
    Download::State _state;
    QString _dbusPath;
    QString _localPath;
    QUrl _url;
    QString _hash;
    QCryptographicHash::Algorithm _algo;
    QVariantMap _metadata;
    QMap<QString, QString> _headers;
    SystemNetworkInfo* _networkInfo;
    RequestFactory* _requestFactory;
    ProcessFactory* _processFactory;
    NetworkReply* _reply;
    QFile* _currentData;
    Download* q_ptr;

};

DownloadPrivate::DownloadPrivate(const QUuid& id, const QString& path, const QUrl& url, const QVariantMap& metadata,
    const QMap<QString, QString>& headers, SystemNetworkInfo* networkInfo, RequestFactory* nam, ProcessFactory* processFactory,
    Download* parent):
        _id(id),
        _totalSize(0),
        _throttle(0),
        _allowGSMDownload(true),
        _state(Download::IDLE),
        _dbusPath(path),
        _url(url),
        _hash(""),
        _algo(QCryptographicHash::Md5),
        _metadata(metadata),
        _headers(headers),
        _networkInfo(networkInfo),
        _requestFactory(nam),
        _processFactory(processFactory),
        q_ptr(parent)
{
    init();
}

DownloadPrivate::DownloadPrivate(const QUuid& id, const QString& path, const QUrl& url, const QString& hash,
    QCryptographicHash::Algorithm algo, const QVariantMap& metadata, const QMap<QString, QString>& headers,
    SystemNetworkInfo* networkInfo, RequestFactory* nam, ProcessFactory* processFactory, Download* parent):
        _id(id),
        _totalSize(0),
        _throttle(0),
        _allowGSMDownload(true),
        _state(Download::IDLE),
        _dbusPath(path),
        _url(url),
        _hash(hash),
        _algo(algo),
        _metadata(metadata),
        _headers(headers),
        _networkInfo(networkInfo),
        _requestFactory(nam),
        _processFactory(processFactory),
        q_ptr(parent)
{
    init();
}

DownloadPrivate::~DownloadPrivate()
{
    if (_currentData != NULL)
    {
        _currentData->close();
        delete _currentData;
    }
    if (_reply != NULL)
        delete _reply;
}

void DownloadPrivate::init()
{
    QStringList pathComponents;
    pathComponents << "download_manager" << _id.toString();
    _localPath = XDGBasedir::saveDataPath(pathComponents);
    qDebug() << "File will be downloaded to" << _localPath;

    _reply = NULL;
    _currentData = NULL;

    // store metadata in case we crash or are stopped
    storeMetadata();
}

void DownloadPrivate::connectToReplySignals()
{
    Q_Q(Download);
    if (_reply != NULL)
    {
        q->connect(_reply, SIGNAL(downloadProgress(qint64, qint64)),
            q, SLOT(onDownloadProgress(qint64, qint64)));
        q->connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            q, SLOT(onError(QNetworkReply::NetworkError)));
        q->connect(_reply, SIGNAL(finished()),
            q, SLOT(onFinished()));
        q->connect(_reply, SIGNAL(sslErrors ( const QList<QSslError>&)),
            q, SLOT(onSslErrors(const QList<QSslError>&)));
    }
}

void DownloadPrivate::disconnectFromReplySignals()
{
    Q_Q(Download);
    if (_reply != NULL)
    {
        q->disconnect(_reply, SIGNAL(downloadProgress(qint64, qint64)),
            q, SLOT(onDownloadProgress(qint64, qint64)));
        q->disconnect(_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            q, SLOT(onError(QNetworkReply::NetworkError)));
        q->disconnect(_reply, SIGNAL(finished()),
            q, SLOT(onFinished()));
        q->disconnect(_reply, SIGNAL(sslErrors ( const QList<QSslError>&)),
            q, SLOT(onSslErrors(const QList<QSslError>&)));
    }
}

QString DownloadPrivate::saveFileName()
{
    QString path = _url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = DATA_FILE_NAME;

    QString final_path = _localPath + QDir::separator() + basename;
    return final_path;
}

QString DownloadPrivate::saveMetadataName()
{
    return _localPath + QDir::separator() + METADATA_FILE_NAME;
}

void DownloadPrivate::storeMetadata()
{
    // data might be already be present, therefore we will delete it (due to the total size being wrong)
    QString metadataPath = saveMetadataName();

    QVariantMap app_metadata = metadata();
    // update the metadata with extra info
    app_metadata[STATE] = _state;
    app_metadata[DBUS_PATH] = _dbusPath;
    app_metadata[URL] = _url.toString();
    app_metadata[HASH] = _hash;
    app_metadata[ALGO] = _algo;

    // remove redundant info like the progress (we know the file)
    app_metadata.remove(PROGRESS);

    QJsonDocument* data = new QJsonDocument(QJsonObject::fromVariantMap(app_metadata));
    QFile* file = new QFile(metadataPath);
    // truncate will remove the content of the file if it exists
    file->open(QIODevice::ReadWrite | QIODevice::Truncate);
    file->write(data->toJson());
    file->close();
    delete data;
    delete file;
}

bool DownloadPrivate::removeDir(const QString& dirName)
{
    bool result = true;
    QDir dir(dirName);

    QFlags<QDir::Filter> filter =  QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files;
    if (dir.exists(dirName)) {
        foreach(QFileInfo info, dir.entryInfoList(filter, QDir::DirsFirst)) {
            if (info.isDir()) {
            result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}

void DownloadPrivate::cleanUpCurrentData()
{
    bool success;
    QString fileName;
    if (_currentData)
    {
        // delete the current data, we did cancel.
        fileName = _currentData->fileName();
        success = _currentData->remove();
        _currentData->deleteLater();
        _currentData = NULL;
        if (!success)
            qWarning() << "Error removing" << fileName;
    }
    success = removeDir(_localPath);
    if (!success)
        qWarning() << "Error removing" << _localPath;

}

QNetworkRequest DownloadPrivate::buildRequest()
{
    qDebug() << "Building request for " << _url;
    QNetworkRequest request = QNetworkRequest(_url);
    foreach(const QString& header, _headers.keys())
    {
        QString data = _headers[header];
        if (header.toLower() == "range")
            // do no add the range
            continue;
        request.setRawHeader(header.toUtf8(), _headers[header].toUtf8());
    }
    return request;
}

QUuid DownloadPrivate::downloadId() const
{
    return _id;
}

QString DownloadPrivate::path() const
{
    return _dbusPath;
}

QUrl DownloadPrivate::url() const
{
    return _url;
}

Download::State DownloadPrivate::state()
{
    return _state;
}

void DownloadPrivate::setState(Download::State state)
{
    _state = state;
}

QString DownloadPrivate::filePath()
{
    if (_currentData)
        return _currentData->fileName();
    return saveFileName();
}

QString DownloadPrivate::hash() const
{
    return _hash;
}

QCryptographicHash::Algorithm DownloadPrivate::hashAlgorithm() const
{
    return _algo;
}

QMap<QString, QString> DownloadPrivate::headers() const
{
    return _headers;
}

bool DownloadPrivate::canDownload()
{
    QNetworkInfo::NetworkMode mode = _networkInfo->currentNetworkMode();
    switch(mode){
        case QNetworkInfo::UnknownMode:
            qWarning() << "Network Mode unknown!";
            return _allowGSMDownload;
            break;
        case QNetworkInfo::GsmMode:
        case QNetworkInfo::CdmaMode:
        case QNetworkInfo::WcdmaMode:
        case QNetworkInfo::WimaxMode:
        case QNetworkInfo::TdscdmaMode:
        case QNetworkInfo::LteMode:
            return _allowGSMDownload;
        case QNetworkInfo::WlanMode:
        case QNetworkInfo::EthernetMode:
        case QNetworkInfo::BluetoothMode:
            return true;
        default:
            return false;
    }
}

void DownloadPrivate::cancelDownload()
{
    Q_Q(Download);
    qDebug() << __FUNCTION__ << _url;

    if (_reply != NULL)
    {
        // disconnect so that we do not get useless signals and remove the reply
        disconnectFromReplySignals();
        _reply->abort();
        _reply->deleteLater();
        _reply = NULL;
    }

    // remove current data and metadata
    cleanUpCurrentData();
    emit q->canceled(true);
}

void DownloadPrivate::pauseDownload()
{
    Q_Q(Download);
    qDebug() << __FUNCTION__ << _url;

    if (_reply == NULL)
    {
        // cannot pause because is not running
        qDebug() << "Cannot pause download because reply is NULL";
        qDebug() << "EMIT paused(false)";
        emit q->paused(false);
        return;
    }

    qDebug() << "Pausing download.";
    // we need to disconnect the signals to ensure that they are not emitted due
    // to the operation we are going to perform. We read the data in the reply and
    // store it in a file
    disconnectFromReplySignals();

    // do abort before reading
    _reply->abort();
    _currentData->write(_reply->readAll());
    _reply->deleteLater();
    _reply = NULL;
    qDebug() << "EMIT paused(true)";
    emit q->paused(true);
}

void DownloadPrivate::resumeDownload()
{
    Q_Q(Download);
    qDebug() << __FUNCTION__ << _url;

    if (_reply != NULL)
    {
        // cannot resume because it is already running
        qDebug() << "Cannot resume download because reply != NULL";
        qDebug() << "EMIT resumed(false)";
        emit q->resumed(false);
        return;
    }

    qDebug() << "Resuming download.";
    QNetworkRequest request = buildRequest();

    // overrides the range header, we do not let clients set the range!!!
    qint64 currentDataSize = _currentData->size();
    QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(currentDataSize) + "-";
    request.setRawHeader("Range", rangeHeaderValue);

    _reply = _requestFactory->get(request);
    _reply->setReadBufferSize(_throttle);

    connectToReplySignals();

    qDebug() << "EMIT resumed(true)";
    emit q->resumed(true);
}

void DownloadPrivate::startDownload()
{
    Q_Q(Download);
    qDebug() << __FUNCTION__ << _url;

    if (_reply != NULL)
    {
        // the download was already started, lets say that we did it
        qDebug() << "Cannot start download because reply != NULL";
        qDebug() << "EMIT started(false)";
        emit q->started(true);
        return;
    }

    qDebug() << "Starting download.";
    // create file that will be used to mantain the state of the download when resumed.
    _currentData = new QFile(saveFileName());
    _currentData->open(QIODevice::ReadWrite | QFile::Append);

    // signals should take care or calling deleteLater on the NetworkReply object
    _reply = _requestFactory->get(buildRequest());
    _reply->setReadBufferSize(_throttle);

    connectToReplySignals();
    qDebug() << "EMIT started(true)";
    emit q->started(true);
}

DownloadPrivate* DownloadPrivate::fromMetadata(const QString &path, RequestFactory* nam, Download* parent)
{
    Q_UNUSED(path)
    Q_UNUSED(nam)
    Q_UNUSED(parent)
    return NULL;
}

QVariantMap DownloadPrivate::metadata()
{
    return _metadata;
}

qulonglong DownloadPrivate::progress()
{
    return (_currentData == NULL)?0:_currentData->size();
}

qulonglong DownloadPrivate::totalSize()
{
    return _totalSize;
}

void DownloadPrivate::setThrottle(qulonglong speed)
{
    qDebug() << __FUNCTION__ << _url;

    _throttle = speed;
    if (_reply != NULL)
        _reply->setReadBufferSize(_throttle);
}

qulonglong DownloadPrivate::throttle()
{
    return _throttle;
}

void DownloadPrivate::allowGSMDownload(bool allowed)
{
    Q_Q(Download);
    if (_allowGSMDownload != allowed)
    {
        _allowGSMDownload = allowed;
        // emit the signals so that they q knows what to do
        emit q->stateChanged();
    }
}

bool DownloadPrivate::isGSMDownloadAllowed()
{
    return _allowGSMDownload;
}

void DownloadPrivate::cancel()
{
    Q_Q(Download);
    _state = Download::CANCELED;
    emit q->stateChanged();
}

void DownloadPrivate::pause()
{
    Q_Q(Download);
    _state = Download::PAUSED;
    storeMetadata();
    emit q->stateChanged();
}

void DownloadPrivate::resume()
{
    Q_Q(Download);
    _state = Download::RESUMED;
    storeMetadata();
    emit q->stateChanged();
}

void DownloadPrivate::start()
{
    Q_Q(Download);
    _state = Download::STARTED;
    storeMetadata();
    emit q->stateChanged();
}

void DownloadPrivate::onDownloadProgress(qint64 progress, qint64 bytesTotal)
{
    Q_Q(Download);
    qDebug() << __FUNCTION__ << _url << progress << bytesTotal;

    // do write the current info we have just in case
    _currentData->write(_reply->readAll());
    qulonglong received = _currentData->size();

    if (bytesTotal == -1)
    {
        // we do not know the size of the download, simply return the same for received and for total
        qDebug() << "EMIT progress" << received << received;
        emit q->progress(received, received);
        return;
    }
    else
    {
        if (_totalSize == 0)
        {
            qDebug() << "Updating total size" << bytesTotal;
            qlonglong uBytestTotal = bytesTotal;
            // bytesTotal is different when we have resumed because we are not counting the size that
            // we already downloaded, therefore we only do this once
            // update the metadata
            _totalSize = uBytestTotal;
            storeMetadata();
        }
        qDebug() << "EMIT progress" << received << _totalSize;
        emit q->progress(received, _totalSize);
        return;
    }
}

void DownloadPrivate::onError(QNetworkReply::NetworkError code)
{
    qCritical() << _url << "ERROR:" << ":" << code;
    // get the error data, disconnect and remove the reply and data

    disconnectFromReplySignals();
    _reply->deleteLater();
    _reply = NULL;
    cleanUpCurrentData();

    // TODO: emit error signal
}

void DownloadPrivate::onFinished()
{
    Q_Q(Download);
    qDebug() << __FUNCTION__ << _url;

    // if the hash is present we check it
    if (!_hash.isEmpty())
    {
        _currentData->reset();
        QByteArray data = _currentData->readAll();
        // do calculate the hash of the file so far and ensure that they are the same
        QString fileSig = QString(QCryptographicHash::hash(data, _algo).toHex());
        if (fileSig != _hash)
        {
            qCritical() << "HASH ERROR:" << fileSig << "!=" << _hash;
            _state = Download::FINISHED;
            emit q->stateChanged();
            emit q->error("HASH ERROR");
            return;
        }
    }

    // there are two possible cases, the first, we do no have the metadata info to execute a
    // commnad once the download was finished and that means we are done here else we execute the
    // command AND raise the finish signals once the command was done (or an error ocurred in the
    // command execution.
    if (_metadata.contains(METADATA_COMMAND_KEY))
    {
        // toStringList will return an empty list if it cannot be converted
        QStringList commandData = _metadata[METADATA_COMMAND_KEY].toStringList();
        if (commandData.count() == 0)
        {
            // raise error, command metadata was passed without the commnad
            qCritical() << "COMMAND DATA MISSING";
            _state = Download::FINISHED;
            emit q->stateChanged();
            emit q->error("COMMAND ERROR");
            return;
        }
        else
        {
            // first item of the string list is the commnad, rest is the arguments
            QString command = commandData.at(0);
            commandData.removeAt(0);
            QStringList args;

            foreach(const QString& arg, commandData)
            {
                if (arg == METADATA_COMMAND_FILE_KEY)
                    args << filePath();
                else
                    args << arg;
            }

            Process* postDownloadProcess = _processFactory->createProcess();

            // connect to signals so that we can tell the clients that the operation succeed

            q->connect(postDownloadProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
                q, SLOT(onProcessFinished(int, QProcess::ExitStatus)));
            q->connect(postDownloadProcess, SIGNAL(error(QProcess::ProcessError)),
                q, SLOT(onProcessError(QProcess::ProcessError)));

            qDebug() << "Executing" << command << args;
            postDownloadProcess->start(command, args);
            return;
        }
    }
    else
    {
        _state = Download::FINISHED;
    	qDebug() << "EMIT stateChanged";
        emit q->stateChanged();
    	qDebug() << "EMIT finished" << filePath();
        emit q->finished(filePath());
    }

    // clean the reply
    _reply->deleteLater();
    _reply = NULL;
}

void DownloadPrivate::onSslErrors(const QList<QSslError>& errors)
{
    qDebug() << __FUNCTION__ << _url;
    // TODO: emit ssl errors signal?
    Q_UNUSED(errors);
    Q_Q(Download);
    emit q->error("SSL ERROR");
}

void DownloadPrivate::onProcessError(QProcess::ProcessError error)
{
    // TODO: better error fowarding
    Q_UNUSED(error);
    Q_Q(Download);
    emit q->error("COMMAND ERROR");
}

void DownloadPrivate::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << __FUNCTION__ << exitCode << exitStatus;
    // TODO: send the command exit code and status
    Q_Q(Download);
    _state = Download::FINISHED;
    qDebug() << "EMIT stateChanged";
    emit q->stateChanged();
    qDebug() << "EMIT finished" << filePath();
    emit q->finished(filePath());
}

/**
 * PUBLIC IMPLEMENTATION
 */

Download::Download(const QUuid& id, const QString& path, const QUrl& url, const QVariantMap& metadata,
    const QMap<QString, QString>& headers, SystemNetworkInfo* networkInfo, RequestFactory* nam, ProcessFactory* processFactory, QObject* parent):
        QObject(parent),
        d_ptr(new DownloadPrivate(id, path, url, metadata, headers, networkInfo, nam, processFactory, this))
{
}

Download::Download(const QUuid& id, const QString& path, const QUrl& url, const QString& hash, QCryptographicHash::Algorithm algo,
    const QVariantMap& metadata, const QMap<QString, QString> &headers, SystemNetworkInfo* networkInfo, RequestFactory* nam,
    ProcessFactory* processFactory, QObject* parent):
        QObject(parent),
        d_ptr(new DownloadPrivate(id, path, url, hash, algo, metadata, headers, networkInfo, nam, processFactory, this))
{
}

Download::Download() :
    d_ptr(NULL)
{
}

QUuid Download::downloadId()
{
    Q_D(Download);
    return d->downloadId();
}

QString Download::path()
{
    Q_D(Download);
    return d->path();
}

QUrl Download::url()
{
    Q_D(Download);
    return d->url();
}

Download::State Download::state()
{
    Q_D(Download);
    return d->state();
}

void Download::setState(Download::State state)
{
    Q_D(Download);
    d->setState(state);
}

QString Download::filePath()
{
    Q_D(Download);
    return d->filePath();
}

QString Download::hash()
{
    Q_D(Download);
    return d->hash();
}

QCryptographicHash::Algorithm Download::hashAlgorithm()
{
    Q_D(Download);
    return d->hashAlgorithm();
}

QMap<QString, QString> Download::headers()
{
    Q_D(Download);
    return d->headers();
}

bool Download::canDownload()
{
    Q_D(Download);
    return d->canDownload();
}

void Download::cancelDownload()
{
    Q_D(Download);
    d->cancelDownload();
}

void Download::pauseDownload()
{
    Q_D(Download);
    d->pauseDownload();
}

void Download::resumeDownload()
{
    Q_D(Download);
    d->resumeDownload();
}

void Download::startDownload()
{
    Q_D(Download);
    d->startDownload();
}

Download* Download::fromMetadata(const QString &path, RequestFactory* reqFactory)
{
    Download* appDownload = new Download();
    DownloadPrivate* priv = DownloadPrivate::fromMetadata(path, reqFactory, appDownload);
    if (priv != NULL)
    {
        appDownload->d_ptr = priv;
        return appDownload;
    }
    return NULL;
}

QVariantMap Download::metadata()
{
    Q_D(Download);
    return d->metadata();
}

qulonglong Download::progress()
{
    Q_D(Download);
    return d->progress();
}

qulonglong Download::totalSize()
{
    Q_D(Download);
    return d->totalSize();
}

void Download::setThrottle(qulonglong speed)
{
    Q_D(Download);
    return d->setThrottle(speed);
}

qulonglong Download::throttle()
{
    Q_D(Download);
    return d->throttle();
}

void Download::allowGSMDownload(bool allowed)
{
    Q_D(Download);
    d->allowGSMDownload(allowed);
}

bool Download::isGSMDownloadAllowed()
{
    Q_D(Download);
    return d->isGSMDownloadAllowed();
}

void Download::cancel()
{
    Q_D(Download);
    d->cancel();
}

void Download::pause()
{
    Q_D(Download);
    d->pause();
}

void Download::resume()
{
    Q_D(Download);
    d->resume();
}

void Download::start()
{
    Q_D(Download);
    d->start();
}

#include "moc_download.cpp"

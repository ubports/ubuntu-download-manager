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


/**
 * PRIVATE IMPLEMENATION
 */

class DownloadPrivate
{
    Q_DECLARE_PUBLIC(Download)
public:
    explicit DownloadPrivate(QString appId, QString appName, QString path, QUrl url, RequestFactory* nam,
        Download* parent);
    explicit DownloadPrivate(QString appId, QString appName, QString path, QUrl url, QString hash,
        QCryptographicHash::Algorithm algo, RequestFactory* nam, Download* parent);
    ~DownloadPrivate();

    // public methods
    QString path() const;
    QUrl url() const;
    Download::State state();
    QString filePath();
    QString hash();
    QCryptographicHash::Algorithm hashAlgorithm();

    // methods that do really perform the actions
    void cancelDownload();
    void pauseDownload();
    void resumeDownload();
    void startDownload();
    static DownloadPrivate* fromMetadata(const QString &path, RequestFactory* nam, Download* parent);

    // plublic slots used by public implementation
    QString applicationId() const;
    QString applicationName() const;
    QVariantMap metadata();
    uint progress();
    uint totalSize();
    void cancel();
    void pause();
    void resume();
    void start();

    // slots executed to keep track of the newtork reply
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onError(QNetworkReply::NetworkError code);
    void onFinished();
    void onSslErrors(const QList<QSslError>& errors);

private:
    void init();
    void connectToReplySignals();
    void disconnectFromReplySignals();
    QString saveFileName();
    QString saveMetadataName();
    void storeMetadata();
    bool removeDir(const QString& dirName);
    void cleanUpCurrentData();

private:
    QString _appId;
    QString _appName;
    uint _totalSize;
    Download::State _state;
    QString _dbusPath;
    QString _localPath;
    QUrl _url;
    QString _hash;
    QCryptographicHash::Algorithm _algo;
    RequestFactory* _requestFactory;
    NetworkReply* _reply;
    QFile* _currentData;
    Download* q_ptr;

};

DownloadPrivate::DownloadPrivate(QString appId, QString appName, QString path, QUrl url, RequestFactory* nam,
    Download* parent):
        _appId(appId),
        _appName(appName),
        _totalSize(0),
        _state(Download::IDLE),
        _dbusPath(path),
        _url(url),
        _hash(""),
        _algo(QCryptographicHash::Md5),
        _requestFactory(nam),
        q_ptr(parent)
{
    init();
}

DownloadPrivate::DownloadPrivate(QString appId, QString appName, QString path, QUrl url, QString hash,
    QCryptographicHash::Algorithm algo, RequestFactory* nam, Download* parent):
        _appId(appId),
        _appName(appName),
        _totalSize(0),
        _state(Download::IDLE),
        _dbusPath(path),
        _url(url),
        _hash(hash),
        _algo(algo),
        _requestFactory(nam),
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
    pathComponents << "application_downloader" << _appId;
    _localPath = XDGBasedir::saveDataPath(pathComponents);

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

QString DownloadPrivate::filePath()
{
    if (_currentData)
        return _currentData->fileName();
    return saveFileName();
}

QString DownloadPrivate::hash()
{
    return _hash;
}

QCryptographicHash::Algorithm DownloadPrivate::hashAlgorithm()
{
    return _algo;
}

void DownloadPrivate::cancelDownload()
{
    Q_Q(Download);

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

    if (_reply == NULL)
    {
        // cannot pause because is not running
        emit q->paused(false);
        return;
    }

    // we need to disconnect the signals to ensure that they are not emitted due
    // to the operation we are going to perform. We read the data in the reply and
    // store it in a file
    disconnectFromReplySignals();

    // do abort before reading
    _reply->abort();
    _currentData->write(_reply->readAll());
    _reply->deleteLater();
    _reply = NULL;
    emit q->paused(true);
}

void DownloadPrivate::resumeDownload()
{
    Q_Q(Download);

    if (_reply != NULL)
    {
        // cannot resume because it is already running
        emit q->resumed(false);
        return;
    }

    qint64 currentDataSize = _currentData->size();

    QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(currentDataSize) + "-";
    QNetworkRequest request = QNetworkRequest(_url);
    request.setRawHeader("Range", rangeHeaderValue);
    _reply = _requestFactory->get(request);

    connectToReplySignals();

    emit q->resumed(true);
}

void DownloadPrivate::startDownload()
{
    Q_Q(Download);

    if (_reply != NULL)
    {
        // the download was already started, lets say that we did it
        emit q->started(true);
        return;
    }

    // create file that will be used to mantain the state of the download when resumed.
    // TODO: Use a better name
    _currentData = new QFile(saveFileName());
    _currentData->open(QIODevice::ReadWrite | QFile::Append);

    // signals should take care or calling deleteLater on the NetworkReply object
    _reply = _requestFactory->get(QNetworkRequest(_url));
    connectToReplySignals();
    emit q->started(true);
}

DownloadPrivate* DownloadPrivate::fromMetadata(const QString &path, RequestFactory* nam, Download* parent)
{

    QString metadataPath = path + "/" + METADATA_FILE_NAME;
    QFileInfo metadataInfo(path);

    // we at least most have the metadata path
    if (!metadataInfo.exists())
    {
        qCritical() << "ERROR: File " << metadataPath << "does not exist.";
        return NULL;
    }

    // try to read the metadata so that is loaded in a QJsonDocument and we can retrieve the
    // required info
    QFile* metadataFile = new QFile(metadataPath);
    metadataFile->open(QIODevice::ReadOnly);
    QByteArray metadata = metadataFile->readAll();
    metadataFile->close();
    QJsonObject metadataJson = QJsonDocument::fromBinaryData(metadata).object();
    if (metadataJson.empty())
    {
        qCritical() << "ERROR: Metadata could not be validated.";
        return NULL;
    }

    // assert tha the json object has all the required keys
    QString appId;
    if (metadataJson.contains(ID))
        appId = metadataJson[ID].toString();
    else
    {
        qCritical() << "ERROR: ID is missing from json.";
        return NULL;
    }

    QString appName;
    if (metadataJson.contains(NAME))
        appName = metadataJson[NAME].toString();
    else
    {
        qCritical() << "ERROR: NAME is missing from json.";
        return NULL;
    }

    QString dbusPath;
    if (metadataJson.contains(DBUS_PATH))
    {
        dbusPath = metadataJson[DBUS_PATH].toString();
    }
    else
    {
        qCritical() << "ERROR: DBUS_PATH missing from json.";
    }

    uint totalSize = 0;
    if (metadataJson.contains(SIZE))
    {
        totalSize = metadataJson.value(SIZE).toVariant().toUInt();
    }

    Download::State state = Download::IDLE;
    if (metadataJson.contains(STATE))
    {
        state = (Download::State)metadataJson.value(STATE).toVariant().toInt();
    }

    QUrl url;
    if (metadataJson.contains(URL))
    {
        url = QUrl(metadataJson[URL].toString());
    }
    else
    {
       qCritical() << "ERROR: URL missing form json.";
       return NULL;
    }

    QString hash = "";
    if (metadataJson.contains(HASH))
        hash = metadataJson[HASH].toString();

    QCryptographicHash::Algorithm algo = QCryptographicHash::Md5;
    if (metadataJson.contains(ALGO))
        algo = (QCryptographicHash::Algorithm)metadataJson.value(ALGO).toVariant().toInt();

    DownloadPrivate* appDownload;
    if (hash.isEmpty())
    {
        appDownload = new DownloadPrivate(appId, appName, dbusPath, url, nam, parent);
    }
    else
    {
        appDownload = new DownloadPrivate(appId, appName, dbusPath, url, hash,
            algo, nam, parent);
    }
    appDownload->_totalSize = totalSize;
    appDownload->_state = state;

    QString dataPath = path + "/" + DATA_FILE_NAME;
    QFileInfo dataInfo(dataPath);
    if (dataInfo.exists())
    {
        // create the file and set it
        appDownload->_currentData = new QFile(dataPath);
        appDownload->_currentData->open(QIODevice::ReadWrite | QFile::Append);
    }
    return appDownload;
}

QString DownloadPrivate::applicationId() const
{
    return _appId;
}

QString DownloadPrivate::applicationName() const
{
    return _appName;
}

QVariantMap DownloadPrivate::metadata()
{
    // create a QVarianMap with the required data
    QVariantMap metadata;
    metadata[ID] = _appId;
    metadata[NAME] = _appName;
    metadata[SIZE] = _totalSize;
    if (_currentData == NULL)
        metadata[PROGRESS] = -1;
    else
        metadata[PROGRESS] = _currentData->size();
    return metadata;
}

uint DownloadPrivate::progress()
{
    return (_currentData == NULL)?0:_currentData->size();
}

uint DownloadPrivate::totalSize()
{
    return _totalSize;
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

void DownloadPrivate::onDownloadProgress(qint64, qint64 bytesTotal)
{
    Q_Q(Download);

    // do write the current info we have just in case
    _currentData->write(_reply->readAll());

    // ignore the case of 0 or when we do not know yet the size
    if (!bytesTotal >= 0)
    {
        if (_totalSize == 0)
        {
            // bytesTotal is different when we have resumed because we are not counting the size that
            // we already downloaded, therefore we only do this once
            _totalSize = bytesTotal;
            // update the metadata
            storeMetadata();
        }
        qint64 received = _currentData->size();

        emit q->progress(received, _totalSize);
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
    _state = Download::FINISHED;
    emit q->stateChanged();
    emit q->finished();
    _reply->deleteLater();
    _reply = NULL;

}

void DownloadPrivate::onSslErrors(const QList<QSslError>& errors)
{
    // TODO: emit ssl errors signal?
    Q_UNUSED(errors);
    Q_Q(Download);
    emit q->error("SSL ERROR");
}

/**
 * PUBLIC IMPLEMENTATION
 */

Download::Download(QString appId, QString appName, QString path, QUrl url, RequestFactory* nam, QObject* parent):
    QObject(parent),
    d_ptr(new DownloadPrivate(appId, appName, path, url, nam, this))
{
}

Download::Download(QString appId, QString appName, QString path, QUrl url, QString hash, QCryptographicHash::Algorithm algo,
    RequestFactory* nam, QObject* parent):
        QObject(parent),
        d_ptr(new DownloadPrivate(appId, appName, path, url, hash, algo, nam, this))
{
}

Download::Download() :
    d_ptr(NULL)
{
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

QString Download::applicationId()
{
    Q_D(Download);
    return d->applicationId();
}

QString Download::applicationName()
{
    Q_D(Download);
    return d->applicationName();
}

QVariantMap Download::metadata()
{
    Q_D(Download);
    return d->metadata();
}

uint Download::progress()
{
    Q_D(Download);
    return d->progress();
}

uint Download::totalSize()
{
    Q_D(Download);
    return d->totalSize();
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

#include <QBuffer>
#include <QDir>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QSignalMapper>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include "xdg_basedir.h"
#include "app_download.h"

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

class AppDownloadPrivate
{
    Q_DECLARE_PUBLIC(AppDownload)
public:
    explicit AppDownloadPrivate(QString appId, QString appName, QString path, QUrl url, RequestFactory* nam,
        AppDownload* parent);
    explicit AppDownloadPrivate(QString appId, QString appName, QString path, QUrl url, QString hash,
        QCryptographicHash::Algorithm algo, RequestFactory* nam, AppDownload* parent);
    ~AppDownloadPrivate();

    // public methods
    QString path() const;
    QUrl url() const;
    AppDownload::State state();
    QString filePath();

    // methods that do really perform the actions
    void cancelDownload();
    void pauseDownload();
    void resumeDownload();
    void startDownload();
    static AppDownloadPrivate* fromMetadata(const QString &path, RequestFactory* nam, AppDownload* parent);

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
    void connetToReplySignals();
    void disconnectFromReplySignals();
    QString saveFileName();
    QString saveMetadataName();
    void storeMetadata();
    void cleanUpCurrentData();

private:
    QString _appId;
    QString _appName;
    uint _totalSize;
    AppDownload::State _state;
    QString _dbusPath;
    QString _localPath;
    QUrl _url;
    QString _hash;
    QCryptographicHash::Algorithm _algo;
    RequestFactory* _requestFactory;
    QNetworkReply* _reply;
    QFile* _currentData;
    AppDownload* q_ptr;

};

AppDownloadPrivate::AppDownloadPrivate(QString appId, QString appName, QString path, QUrl url, RequestFactory* nam,
    AppDownload* parent):
        _appId(appId),
        _appName(appName),
        _totalSize(0),
        _state(AppDownload::IDLE),
        _dbusPath(path),
        _url(url),
        _hash(""),
        _algo(QCryptographicHash::Md5),
        _requestFactory(nam),
        q_ptr(parent)
{
    init();
}

AppDownloadPrivate::AppDownloadPrivate(QString appId, QString appName, QString path, QUrl url, QString hash,
    QCryptographicHash::Algorithm algo, RequestFactory* nam, AppDownload* parent):
        _appId(appId),
        _appName(appName),
        _totalSize(0),
        _state(AppDownload::IDLE),
        _dbusPath(path),
        _url(url),
        _hash(hash),
        _algo(algo),
        _requestFactory(nam),
        q_ptr(parent)
{
    init();
}

AppDownloadPrivate::~AppDownloadPrivate()
{
    if (_currentData != NULL)
    {
        qDebug() << "Closing current data file.";
        _currentData->close();
        delete _currentData;
    }
    if (_reply != NULL)
        delete _reply;
}

void AppDownloadPrivate::init()
{
    QStringList pathComponents;
    pathComponents << "application_downloader" << _appId;
    _localPath = XDGBasedir::saveDataPath(pathComponents);

    _reply = NULL;
    _currentData = NULL;

    // store metadata in case we crash or are stopped
    storeMetadata();
}

void AppDownloadPrivate::connetToReplySignals()
{
    Q_Q(AppDownload);
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

void AppDownloadPrivate::disconnectFromReplySignals()
{
    Q_Q(AppDownload);
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

QString AppDownloadPrivate::saveFileName()
{
    QString path = _url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = DATA_FILE_NAME;

    QString final_path = _localPath + QDir::separator() + basename;
    qDebug() << "Save path name is" << final_path;
    return final_path;
}

QString AppDownloadPrivate::saveMetadataName()
{
    return _localPath + QDir::separator() + METADATA_FILE_NAME;
}

void AppDownloadPrivate::storeMetadata()
{
    // data might be already be present, therefore we will delete it (due to the total size being wrong)
    QString metadataPath = saveMetadataName();
    qDebug() << "Storing metadata in" << metadataPath << "for app" << _appName;

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

void AppDownloadPrivate::cleanUpCurrentData()
{
    bool success;
    QString fileName;
    if (_currentData)
    {
        // delete the current data, we did cancel.
        fileName = _currentData->fileName();
        qDebug() << "Removing file" << fileName;
        success = _currentData->remove();
        _currentData->deleteLater();
        _currentData = NULL;
        if (!success)
            qWarning() << "Error removing" << fileName;
    }
    else
    {
        fileName = saveFileName();
        // check if the file is present and remove it
        QFileInfo fileInfo = QFileInfo(fileName);

        if (fileInfo.exists())
        {
            qDebug() << "Removing file" << fileName;
            success = QFile::remove(fileInfo.absoluteFilePath());
            if (!success)
                qWarning() << "Error removing" << fileName;
        }
    }
    // remove metadata and dir
    fileName = saveMetadataName();
    QFileInfo metadataInfo = QFileInfo(fileName);

    if (metadataInfo.exists())
    {
        qDebug() << "Removing file" << fileName;
        success = QFile::remove(metadataInfo.absoluteFilePath());
        if (!success)
            qWarning() << "Error removing" << fileName;
    }

    QDir dir(_localPath);
    if (dir.exists())
    {
        success = dir.remove(_localPath);
        if (!success)
            qWarning() << "Error removing" << _localPath;
    }
}

QString AppDownloadPrivate::path() const
{
    return _dbusPath;
}

QUrl AppDownloadPrivate::url() const
{
    return _url;
}

AppDownload::State AppDownloadPrivate::state()
{
    return _state;
}

QString AppDownloadPrivate::filePath()
{
    if (_currentData)
        return _currentData->fileName();
    return "";
}

void AppDownloadPrivate::cancelDownload()
{
    Q_Q(AppDownload);

    qDebug() << "Canceling download for " << _url;

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

void AppDownloadPrivate::pauseDownload()
{
    Q_Q(AppDownload);

    if (_reply == NULL)
    {
        // cannot pause because is not running
        emit q->paused(false);
        return;
    }

    qDebug() << "Pausing download for " << _url;
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

void AppDownloadPrivate::resumeDownload()
{
    Q_Q(AppDownload);

    if (_reply != NULL)
    {
        // cannot resume because it is already running
        emit q->resumed(false);
        return;
    }

    qint64 currentDataSize = _currentData->size();
    qDebug() << "Resuming download for " << _url << "at" << currentDataSize;

    QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(currentDataSize) + "-";
    QNetworkRequest request = QNetworkRequest(_url);
    request.setRawHeader("Range",rangeHeaderValue);
    _reply = _requestFactory->get(request);

    connetToReplySignals();

    emit q->resumed(true);
}

void AppDownloadPrivate::startDownload()
{
    Q_Q(AppDownload);

    if (_reply != NULL)
    {
        // the download was already started, lets say that we did it
        emit q->started(true);
        return;
    }

    qDebug() << "START:" << _url;

    // create file that will be used to mantain the state of the download when resumed.
    // TODO: Use a better name
    _currentData = new QFile(saveFileName());
    _currentData->open(QIODevice::ReadWrite | QFile::Append);
    qDebug() << "IO Device created in " << _currentData->fileName();

    // signals should take care or calling deleteLater on the QNetworkReply object
    _reply = _requestFactory->get(QNetworkRequest(_url));
    connetToReplySignals();
    emit q->started(true);
}

AppDownloadPrivate* AppDownloadPrivate::fromMetadata(const QString &path, RequestFactory* nam, AppDownload* parent)
{
    qDebug() << "Loading app download from metadata";

    QString metadataPath = path + "/" + METADATA_FILE_NAME;
    QFileInfo metadataInfo(path);

    // we at least most have the metadata path
    if (!metadataInfo.exists())
    {
        qDebug() << "ERROR: File " << metadataPath << "does not exist.";
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
        qDebug() << "ERROR: Metadata could not be validated.";
        return NULL;
    }

    // assert tha the json object has all the required keys
    QString appId;
    if (metadataJson.contains(ID))
        appId = metadataJson[ID].toString();
    else
    {
        qDebug() << "ERROR: ID is missing from json.";
        return NULL;
    }

    QString appName;
    if (metadataJson.contains(NAME))
        appName = metadataJson[NAME].toString();
    else
    {
        qDebug() << "ERROR: NAME is missing from json.";
        return NULL;
    }

    QString dbusPath;
    if (metadataJson.contains(DBUS_PATH))
    {
        dbusPath = metadataJson[DBUS_PATH].toString();
    }
    else
    {
        qDebug() << "ERROR: DBUS_PATH missing from json.";
    }

    uint totalSize = 0;
    if (metadataJson.contains(SIZE))
    {
        totalSize = metadataJson.value(SIZE).toVariant().toUInt();
    }

    AppDownload::State state = AppDownload::IDLE;
    if (metadataJson.contains(STATE))
    {
        state = (AppDownload::State)metadataJson.value(STATE).toVariant().toInt();
    }

    QUrl url;
    if (metadataJson.contains(URL))
    {
        url = QUrl(metadataJson[URL].toString());
    }
    else
    {
       qDebug() << "ERROR: URL missing form json.";
       return NULL;
    }

    QString hash = "";
    if (metadataJson.contains(HASH))
        hash = metadataJson[HASH].toString();

    QCryptographicHash::Algorithm algo = QCryptographicHash::Md5;
    if (metadataJson.contains(ALGO))
        algo = (QCryptographicHash::Algorithm)metadataJson.value(ALGO).toVariant().toInt();

    AppDownloadPrivate* appDownload;
    if (hash.isEmpty())
    {
        appDownload = new AppDownloadPrivate(appId, appName, dbusPath, url, nam, parent);
    }
    else
    {
        appDownload = new AppDownloadPrivate(appId, appName, dbusPath, url, hash,
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

QString AppDownloadPrivate::applicationId() const
{
    return _appId;
}

QString AppDownloadPrivate::applicationName() const
{
    return _appName;
}

QVariantMap AppDownloadPrivate::metadata()
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

uint AppDownloadPrivate::progress()
{
    return (_currentData == NULL)?-1:_currentData->size();
}

uint AppDownloadPrivate::totalSize()
{
    return _totalSize;
}

void AppDownloadPrivate::cancel()
{
    Q_Q(AppDownload);
    qDebug() << "CANCELED:" << _url;
    _state = AppDownload::CANCELED;
    emit q->stateChanged();
}

void AppDownloadPrivate::pause()
{
    Q_Q(AppDownload);
    qDebug() << "PAUSED:" << _url;
    _state = AppDownload::PAUSED;
    storeMetadata();
    emit q->stateChanged();
}

void AppDownloadPrivate::resume()
{
    Q_Q(AppDownload);
    qDebug() << "RESUMED:" << _url;
    _state = AppDownload::RESUMED;
    storeMetadata();
    emit q->stateChanged();
}

void AppDownloadPrivate::start()
{
    Q_Q(AppDownload);
    qDebug() << "STARTED:" << _url;
    _state = AppDownload::STARTED;
    storeMetadata();
    emit q->stateChanged();
}

void AppDownloadPrivate::onDownloadProgress(qint64, qint64 bytesTotal)
{
    Q_Q(AppDownload);

    // do write the current info we have just in case
    _currentData->write(_reply->readAll());

    // ignore the case of 0 or when we do not know yet the size
    if (!bytesTotal >= 0)
    {
        if (_totalSize == 0)
        {
            // bytesTotal is different when we have resumed because we are not counting the size that
            // we already downloaded, therefore we only do this once
            qDebug() << "Setting total size to " << _totalSize;
            _totalSize = bytesTotal;
            // update the metadata
            storeMetadata();
        }
        qint64 received = _currentData->size();

        qDebug() << _url << "PROGRESS: " << received << "/" << bytesTotal;
        emit q->progress(received, _totalSize);
    }
}

void AppDownloadPrivate::onError(QNetworkReply::NetworkError code)
{
    qDebug() << _url << "ERROR:" << ":" << code;
    // get the error data, disconnect and remove the reply and data

    disconnectFromReplySignals();
    _reply->deleteLater();
    _reply = NULL;
    cleanUpCurrentData();

    // TODO: emit error signal
}

void AppDownloadPrivate::onFinished()
{
    Q_Q(AppDownload);

    qDebug() << _url << "FINIHSED";
    _currentData->reset();
    QByteArray data = _currentData->readAll();

    // if the hash is present we check it
    if (!_hash.isEmpty())
    {
        // do calculate the hash of the file so far and ensure that they are the same
        QCryptographicHash hash(_algo);
        hash.addData(data);
        QByteArray fileSig = hash.result();
        QByteArray originalSig = QByteArray::fromHex(_hash.toLatin1());
        if (fileSig != originalSig)
        {
            qDebug() << "HASH ERROR:" << fileSig << "!=" << originalSig;
            emit q->error("HASH ERROR");
            return;
        }
    }
    emit q->finished();
    _reply->deleteLater();
    _reply = NULL;

}

void AppDownloadPrivate::onSslErrors(const QList<QSslError>& errors)
{
    qDebug() << _url << "SLL ERRORS:" << errors.count();
    // TODO: emit ssl errors signal?
}

/**
 * PUBLIC IMPLEMENTATION
 */

AppDownload::AppDownload(QString appId, QString appName, QString path, QUrl url, RequestFactory* nam, QObject* parent):
    QObject(parent),
    d_ptr(new AppDownloadPrivate(appId, appName, path, url, nam, this))
{
}

AppDownload::AppDownload(QString appId, QString appName, QString path, QUrl url, QString hash, QCryptographicHash::Algorithm algo,
    RequestFactory* nam, QObject* parent):
        QObject(parent),
        d_ptr(new AppDownloadPrivate(appId, appName, path, url, hash, algo, nam, this))
{
}

AppDownload::AppDownload() :
    d_ptr(NULL)
{
}

QString AppDownload::path()
{
    Q_D(AppDownload);
    return d->path();
}

QUrl AppDownload::url()
{
    Q_D(AppDownload);
    return d->url();
}

AppDownload::State AppDownload::state()
{
    Q_D(AppDownload);
    return d->state();
}

QString AppDownload::filePath()
{
    Q_D(AppDownload);
    return d->filePath();
}

void AppDownload::cancelDownload()
{
    Q_D(AppDownload);
    d->cancelDownload();
}

void AppDownload::pauseDownload()
{
    Q_D(AppDownload);
    d->pauseDownload();
}

void AppDownload::resumeDownload()
{
    Q_D(AppDownload);
    d->resumeDownload();
}

void AppDownload::startDownload()
{
    Q_D(AppDownload);
    d->startDownload();
}

AppDownload* AppDownload::fromMetadata(const QString &path, RequestFactory* reqFactory)
{
    AppDownload* appDownload = new AppDownload();
    AppDownloadPrivate* priv = AppDownloadPrivate::fromMetadata(path, reqFactory, appDownload);
    if (priv != NULL)
    {
        appDownload->d_ptr = priv;
        return appDownload;
    }
    return NULL;
}

QString AppDownload::applicationId()
{
    Q_D(AppDownload);
    return d->applicationId();
}

QString AppDownload::applicationName()
{
    Q_D(AppDownload);
    return d->applicationName();
}

QVariantMap AppDownload::metadata()
{
    Q_D(AppDownload);
    return d->metadata();
}

uint AppDownload::progress()
{
    Q_D(AppDownload);
    return d->progress();
}

uint AppDownload::totalSize()
{
    Q_D(AppDownload);
    return d->totalSize();
}

void AppDownload::cancel()
{
    Q_D(AppDownload);
    d->cancel();
}

void AppDownload::pause()
{
    Q_D(AppDownload);
    d->pause();
}

void AppDownload::resume()
{
    Q_D(AppDownload);
    d->resume();
}

void AppDownload::start()
{
    Q_D(AppDownload);
    d->start();
}


#include "moc_app_download.cpp"

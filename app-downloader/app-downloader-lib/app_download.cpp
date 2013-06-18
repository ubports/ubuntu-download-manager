#include <QBuffer>
#include <QDebug>
#include <QNetworkReply>
#include <QSignalMapper>
#include <QTemporaryFile>
#include "app_download.h"



/**
 * PRIVATE IMPLEMENATION
 */

class AppDownloadPrivate
{
    Q_DECLARE_PUBLIC(AppDownload)
public:
    explicit AppDownloadPrivate(QString appId, QString appName, QString path, QUrl url, QNetworkAccessManager* nam,
        AppDownload* parent);
    explicit AppDownloadPrivate(QString appId, QString appName, QString path, QUrl url, QString hash,
        QCryptographicHash::Algorithm algo, QNetworkAccessManager* nam, AppDownload* parent);

    // public methods
    QString path() const;
    QUrl url() const;
    AppDownload::State state();

    // methods that do really perform the actions
    void cancelDownload();
    void pauseDownload();
    void resumeDownload();
    void startDownload();

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
    void connetToReplySignals();
    void disconnectFromReplySignals();

private:
    QString _appId;
    QString _appName;
    uint _totalSize;
    AppDownload::State _state;
    QString _path;
    QUrl _url;
    QString _hash;
    QCryptographicHash::Algorithm _algo;
    QNetworkAccessManager* _nam;
    QNetworkReply* _reply;
    QTemporaryFile* _currentData;
    AppDownload* q_ptr;

};

AppDownloadPrivate::AppDownloadPrivate(QString appId, QString appName, QString path, QUrl url, QNetworkAccessManager* nam,
    AppDownload* parent):
        _appId(appId),
        _appName(appName),
        _totalSize(0),
        _state(AppDownload::IDLE),
        _path(path),
        _url(url),
        _hash(""),
        _nam(nam),
        q_ptr(parent)
{
    _reply = NULL;
    _currentData = NULL;
}

AppDownloadPrivate::AppDownloadPrivate(QString appId, QString appName, QString path, QUrl url, QString hash,
    QCryptographicHash::Algorithm algo, QNetworkAccessManager* nam, AppDownload* parent):
        _appId(appId),
        _appName(appName),
        _totalSize(0),
        _state(AppDownload::IDLE),
        _path(path),
        _url(url),
        _hash(hash),
        _algo(algo),
        _nam(nam),
        q_ptr(parent)
{
    _reply = NULL;
    _currentData = NULL;
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

QString AppDownloadPrivate::path() const
{
    return _path;
}

QUrl AppDownloadPrivate::url() const
{
    return _url;
}

AppDownload::State AppDownloadPrivate::state()
{
    return _state;
}

void AppDownloadPrivate::cancelDownload()
{
    Q_Q(AppDownload);

    if (_reply == NULL)
    {
        // cannot run because it is not running
        emit q->canceled(false);
        return;
    }

    qDebug() << "Canceling download for " << _url;

    // disconnect so that we do not get useless signals and rremove the reply
    disconnectFromReplySignals();
    _reply->abort();
    _reply->deleteLater();
    _reply = NULL;

    // delete the current data, we did cancel.
    _currentData->deleteLater();
    _currentData = NULL;

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

    _reply = _nam->get(request);
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
    // create temp file that will be used to mantain the state of the download when resumed.
    _currentData = new QTemporaryFile();
    _currentData->open();
    qDebug() << "Tempp IO Device created in " << _currentData->fileName();

    // signals should take care or calling deleteLater on the QNetworkReply object
    _reply = _nam->get(QNetworkRequest(_url));
    connetToReplySignals();
    emit q->started(true);
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
    metadata["id"] = _appId;
    metadata["name"] = _appName;
    metadata["size"] = _totalSize;
    if (_currentData == NULL)
        metadata["progress"] = -1;
    else
        metadata["progress"] = _currentData->size();
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
    emit q->stateChanged();
}

void AppDownloadPrivate::resume()
{
    Q_Q(AppDownload);
    qDebug() << "RESUMED:" << _url;
    _state = AppDownload::RESUMED;
    emit q->stateChanged();
}

void AppDownloadPrivate::start()
{
    Q_Q(AppDownload);
    qDebug() << "STARTED:" << _url;
    _state = AppDownload::STARTED;
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
        }
        qint64 received = _currentData->size();

        qDebug() << _url << "PROGRESS: " << received << "/" << bytesTotal;
        emit q->progress(received, _totalSize);
    }
}

void AppDownloadPrivate::onError(QNetworkReply::NetworkError code)
{
    qDebug() << _url << "ERROR:" << ":" << code;
    // get the error data, disconnect and remove the reply

    disconnectFromReplySignals();
    _reply->deleteLater();
    _reply = NULL;
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
}

/**
 * PUBLIC IMPLEMENTATION
 */

AppDownload::AppDownload(QString appId, QString appName, QString path, QUrl url, QNetworkAccessManager* nam, QObject* parent):
    QObject(parent),
    d_ptr(new AppDownloadPrivate(appId, appName, path, url, nam, this))
{
}

AppDownload::AppDownload(QString appId, QString appName, QString path, QUrl url, QString hash, QCryptographicHash::Algorithm algo,
    QNetworkAccessManager* nam, QObject* parent):
        QObject(parent),
        d_ptr(new AppDownloadPrivate(appId, appName, path, url, hash, algo, nam, this))
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

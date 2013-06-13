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
    explicit AppDownloadPrivate(QString path, QUrl url, QNetworkAccessManager* nam, AppDownload* parent);
    explicit AppDownloadPrivate(QString path, QUrl url, QByteArray* hash, QNetworkAccessManager* nam, AppDownload* parent);

    // public methods
    QString path();
    QUrl url();

    // plublic slots used by public implementation
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
    QString _path;
    QUrl _url;
    QByteArray* _hash;
    QNetworkAccessManager* _nam;
    QNetworkReply* _reply;
    QTemporaryFile* _currentData;
    AppDownload* q_ptr;

};

AppDownloadPrivate::AppDownloadPrivate(QString path, QUrl url, QNetworkAccessManager* nam, AppDownload* parent):
    _path(path),
    _url(url),
    _nam(nam),
    q_ptr(parent)
{
    _hash = NULL;
    _reply = NULL;
    _currentData = NULL;
}

AppDownloadPrivate::AppDownloadPrivate(QString path, QUrl url, QByteArray* hash, QNetworkAccessManager* nam, AppDownload* parent):
    _path(path),
    _url(url),
    _hash(hash),
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

QString AppDownloadPrivate::path()
{
    return _path;
}

QUrl AppDownloadPrivate::url()
{
    return _url;
}

void AppDownloadPrivate::cancel()
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

void AppDownloadPrivate::pause()
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

void AppDownloadPrivate::resume()
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

void AppDownloadPrivate::start()
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

void AppDownloadPrivate::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_Q(AppDownload);

    // do write the current info we have just in case
    _currentData->write(_reply->readAll());

    // ignore the case of 0 or when we do not know yet the size
    if (!bytesTotal >= 0)
    {
        qDebug() << _url << "PROGRESS: " << bytesReceived << "/" << bytesTotal;
        emit q->progress(bytesReceived, bytesTotal);
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
    // if the hash is present we check it
    if (_hash)
    {
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

AppDownload::AppDownload(QString path, QUrl url, QNetworkAccessManager* nam, QObject* parent):
    QObject(parent),
    d_ptr(new AppDownloadPrivate(path, url, nam, this))
{
}

AppDownload::AppDownload(QString path, QUrl url, QByteArray* hash, QNetworkAccessManager* nam, QObject* parent):
    QObject(parent),
    d_ptr(new AppDownloadPrivate(path, url, hash, nam, this))
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

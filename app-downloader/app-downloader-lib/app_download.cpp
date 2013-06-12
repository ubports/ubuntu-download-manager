#include <QBuffer>
#include <QDebug>
#include <QNetworkReply>
#include <QSignalMapper>
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

    QString path();
    QUrl url();

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
    QString _path;
    QUrl _url;
    QByteArray* _hash;
    QNetworkAccessManager* _nam;
    QNetworkReply* _reply;
    bool _wasAborted;
    AppDownload* q_ptr;

};

AppDownloadPrivate::AppDownloadPrivate(QString path, QUrl url, QNetworkAccessManager* nam, AppDownload* parent):
    _path(path),
    _url(url),
    _nam(nam),
    _wasAborted(false),
    q_ptr(parent)
{
    _hash = NULL;
    _reply = NULL;
}

AppDownloadPrivate::AppDownloadPrivate(QString path, QUrl url, QByteArray* hash, QNetworkAccessManager* nam, AppDownload* parent):
    _path(path),
    _url(url),
    _hash(hash),
    _nam(nam),
    _wasAborted(false),
    q_ptr(parent)
{
    _reply = NULL;
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
        q->canceled(false);
    }

    qDebug() << "Canceling download for " << _url;
    _wasAborted = true;
    _reply->abort();
    _reply->deleteLater();
    _reply = NULL;
}

void AppDownloadPrivate::pause()
{
    Q_Q(AppDownload);

    if (_reply == NULL)
    {
        // cannot pause because is not running
        q->paused(false);
    }

    qDebug() << "Pausing download for " << _url;
}

void AppDownloadPrivate::resume()
{
    Q_Q(AppDownload);

    if (_reply != NULL)
    {
        // cannot resume because it is already running
        q->resumed(false);
    }

    qDebug() << "Resuming download for " << _url;
}

void AppDownloadPrivate::start()
{
    Q_Q(AppDownload);

    if (_reply != NULL)
    {
        // the download was already started, lets say that we did it
        q->started(true);
    }

    qDebug() << "START:" << _url;

    // create the qnetwork reply and connect to the signals
    _reply = _nam->get(QNetworkRequest(_url));

    // signals should take care or calling deleteLater on the QNetworkReply object
    q->connect(_reply, SIGNAL(downloadProgress(qint64, qint64)),
        q, SLOT(onDownloadProgress(qint64, qint64)));
    q->connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)),
        q, SLOT(onError(QNetworkReply::NetworkError)));
    q->connect(_reply, SIGNAL(finished()),
        q, SLOT(onFinished()));
    q->connect(_reply, SIGNAL(sslErrors ( const QList<QSslError>&)),
        q, SLOT(onSslErrors(const QList<QSslError>&)));

}

void AppDownloadPrivate::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    qDebug() << _url << "PROGRESS: " << bytesReceived << "/" << bytesTotal;
}

void AppDownloadPrivate::onError(QNetworkReply::NetworkError code)
{
    qDebug() << _url << "ERROR:" << ":" << code;
}

void AppDownloadPrivate::onFinished()
{
    Q_Q(AppDownload);

    qDebug() << _url << "FINIHSED";
    // if the has is present we check it
    if (_hash)
    {
    }
    q->finished();

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

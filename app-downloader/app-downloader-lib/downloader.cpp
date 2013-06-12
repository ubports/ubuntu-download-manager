#include "downloader.h"
#include <QNetworkAccessManager>
#include <QSignalMapper>

/**
 * PRIVATE IMPLEMENATION
 */

class DownloaderPrivate
{
    Q_DECLARE_PUBLIC(Downloader)
public:
    explicit DownloaderPrivate(Downloader* parent);

    QSharedPointer<AppDownload> getApplication(QUrl url);
    QSharedPointer<AppDownload> getApplication(QUrl url, QByteArray* hash);

private:
    Downloader* q_ptr;
    QNetworkAccessManager* _nam;
    QSignalMapper* _mapper;

};

DownloaderPrivate::DownloaderPrivate(Downloader* parent):
    q_ptr(parent),
    _nam(),
    _mapper()
{
}

QSharedPointer<AppDownload> DownloaderPrivate::getApplication(QUrl url)
{
    Q_Q(Downloader);

    QSharedPointer<AppDownload> appDown = QSharedPointer<AppDownload>(new AppDownload());
    QNetworkReply* reply = _nam->get(QNetworkRequest(url));

    // signals should take care or calling deleteLater on the QNetworkReply object
    q->connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
            appDown.data(), SLOT(onDownloadProgress(qint64, qint64)));
    q->connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            appDown.data(), SLOT(onError(QNetworkReply::NetworkError)));
    q->connect(reply, SIGNAL(finished()),
            appDown.data(), SLOT(onFinished()));
    q->connect(reply, SIGNAL(sslErrors ( const QList<QSslError>&)),
            appDown.data(), SLOT(onSslErrors(const QList<QSslError>&)));

    return appDown;

}

QSharedPointer<AppDownload> DownloaderPrivate::getApplication(QUrl url, QByteArray* hash)
{
    Q_Q(Downloader);
    QSharedPointer<AppDownload> appDown = QSharedPointer<AppDownload>(new AppDownload());
    QNetworkReply* reply = _nam->get(QNetworkRequest(url));

    q->connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
        appDown.data(), SLOT(onDownloadProgress(qint64, qint64)));
    q->connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
        appDown.data(), SLOT(onError(QNetworkReply::NetworkError)));
    q->connect(reply, SIGNAL(sslErrors ( const QList<QSslError>&)),
        appDown.data(), SLOT(onSslErrors(const QList<QSslError>&)));

    // use the mapper so that we can pass the hash to the finished signal
    q->connect(reply, SIGNAL(finished()), _mapper, SLOT(map()));

    // use QBuffer because signal mapper is 'stupid' and needs to take QObjects
    _mapper->setMapping(reply, new QBuffer(hash));

    q->connect(_mapper, SIGNAL(mapper(QObject*)),
        appDown.data(), SIGNAL());


    return appDown;
}

/**
 * PUBLIC IMPLEMENTATION
 */

Downloader::Downloader(QObject *parent) :
    QObject(parent),
    d_ptr(new DownloaderPrivate(this))
{
}

QSharedPointer<AppDownload> Downloader::getApplication(QUrl url)
{
    Q_D(Downloader);
    return d->getApplication(url);
}

QSharedPointer<AppDownload> Downloader::getApplication(QUrl url, QByteArray* hash)
{
    Q_D(Downloader);
    return d->getApplication(url, hash);
}

#include "moc_downloader.cpp"

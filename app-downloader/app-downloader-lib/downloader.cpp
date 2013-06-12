#include <QUuid>
#include <QRegExp>
#include <QNetworkAccessManager>
#include <QSignalMapper>
#include "downloader.h"

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
    QString buildDownloadPath();

private:
    static QString BASE_ACCOUNT_URL;

    Downloader* q_ptr;
    QNetworkAccessManager* _nam;

};

QString DownloaderPrivate::BASE_ACCOUNT_URL = "/com/canonical/applications/download/%1";

DownloaderPrivate::DownloaderPrivate(Downloader* parent):
    q_ptr(parent)
{
    _nam = new QNetworkAccessManager();
}

QString DownloaderPrivate::buildDownloadPath()
{
    // use a uuid to uniquely identify the downloader
    QUuid uuid = QUuid::createUuid();
    QString uuidString = uuid.toString().replace(QRegExp("[-{}]"), "");
    return DownloaderPrivate::BASE_ACCOUNT_URL.arg(uuidString);
}

QSharedPointer<AppDownload> DownloaderPrivate::getApplication(QUrl url)
{
    QString path = buildDownloadPath();
    QSharedPointer<AppDownload> appDown = QSharedPointer<AppDownload>(new AppDownload(path, url, _nam));
    return appDown;
}

QSharedPointer<AppDownload> DownloaderPrivate::getApplication(QUrl url, QByteArray* hash)
{
    QString path = buildDownloadPath();
    QSharedPointer<AppDownload> appDown = QSharedPointer<AppDownload>(new AppDownload(path, url, hash, _nam));
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

#include <QUuid>
#include <QRegExp>
#include <QNetworkAccessManager>
#include <QSignalMapper>
#include "application_download_adaptor.h"
#include "downloader.h"

/**
 * PRIVATE IMPLEMENATION
 */

class DownloaderPrivate
{
    Q_DECLARE_PUBLIC(Downloader)
public:
    explicit DownloaderPrivate(QDBusConnection connection, Downloader* parent);

private:
    QString buildDownloadPath();
    AppDownload* getApplication(QUrl url);
    AppDownload* getApplication(QUrl url, QByteArray* hash);
    QDBusObjectPath createDownload(const QString &url);

private:
    static QString BASE_ACCOUNT_URL;

    QHash<QString, QPair<AppDownload*,  ApplicationDownloadAdaptor*> > _downloads;
    QDBusConnection _conn;
    Downloader* q_ptr;
    QNetworkAccessManager* _nam;

};

QString DownloaderPrivate::BASE_ACCOUNT_URL = "/com/canonical/applications/download/%1";

DownloaderPrivate::DownloaderPrivate(QDBusConnection connection, Downloader* parent):
    _conn(connection),
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

AppDownload* DownloaderPrivate::getApplication(QUrl url)
{
    QString path = buildDownloadPath();
    AppDownload* appDown = new AppDownload(path, url, _nam);
    return appDown;
}

AppDownload* DownloaderPrivate::getApplication(QUrl url, QByteArray* hash)
{
    QString path = buildDownloadPath();
    AppDownload* appDown = new AppDownload(path, url, hash, _nam);
    return appDown;
}

QDBusObjectPath DownloaderPrivate::createDownload(const QString &url)
{
    Q_Q(Downloader);
    qDebug() << "Creating AppDownload object for " << url;
    AppDownload* appDownload = getApplication(url);
    ApplicationDownloadAdaptor* adaptor = new ApplicationDownloadAdaptor(appDownload);

    // we need to store the ref of both objects, else the mem management will delete them
    _downloads[appDownload->path()] = QPair<AppDownload*, ApplicationDownloadAdaptor*>(appDownload, adaptor);
    bool ret = _conn.registerObject(appDownload->path(), appDownload);
    qDebug() << "New DBus object registered to " << appDownload->path() << ret;

    // emit that the download was created. Usefull in case other processes are interested in them
    emit q->downloadCreated(appDownload->path());
    return QDBusObjectPath(appDownload->path());
}

/**
 * PUBLIC IMPLEMENTATION
 */

Downloader::Downloader(QDBusConnection connection, QObject *parent) :
    QObject(parent),
    d_ptr(new DownloaderPrivate(connection, this))
{
}

QDBusObjectPath Downloader::createDownload(const QString &url)
{
    Q_D(Downloader);
    return d->createDownload(url);
}

#include "moc_downloader.cpp"

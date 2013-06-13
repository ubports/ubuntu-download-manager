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
    AppDownload* getApplication(QUrl url, QString hash, QCryptographicHash::Algorithm algo);

    QDBusObjectPath createDownload(const QString &url);
    QDBusObjectPath createDownloadWithHash(const QString &url, const QString &hash, QCryptographicHash::Algorithm algo);
    QList<QDBusObjectPath> getAllDownloads();

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

AppDownload* DownloaderPrivate::getApplication(QUrl url, QString hash, QCryptographicHash::Algorithm algo)
{
    QString path = buildDownloadPath();
    AppDownload* appDown = new AppDownload(path, url, hash, algo, _nam);
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
    QDBusObjectPath objectPath(appDownload->path());
    emit q->downloadCreated(objectPath);
    return objectPath;
}

QDBusObjectPath DownloaderPrivate::createDownloadWithHash(const QString &url, const QString &hash, QCryptographicHash::Algorithm algo)
{
    Q_Q(Downloader);
    qDebug() << "Creating AppDownload object for " << url << "hash" << hash << "algo" << algo;
    AppDownload* appDownload = getApplication(url, hash, algo);
    ApplicationDownloadAdaptor* adaptor = new ApplicationDownloadAdaptor(appDownload);

    // we need to store the ref of both objects, else the mem management will delete them
    _downloads[appDownload->path()] = QPair<AppDownload*, ApplicationDownloadAdaptor*>(appDownload, adaptor);
    bool ret = _conn.registerObject(appDownload->path(), appDownload);
    qDebug() << "New DBus object registered to " << appDownload->path() << ret;

    // emit that the download was created. Usefull in case other processes are interested in them
    QDBusObjectPath objectPath(appDownload->path());
    emit q->downloadCreated(objectPath);
    return objectPath;
}

QList<QDBusObjectPath> DownloaderPrivate::getAllDownloads()
{
    Q_Q(Downloader);
    qDebug() << "Getting all object paths.";

    QList<QDBusObjectPath> paths;
    foreach(const QString &key, _downloads.keys())
        paths << QDBusObjectPath(key);

    // emit the signal in case any other process is interested
    emit q->downloads(paths);
    return paths;
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

QDBusObjectPath Downloader::createDownloadMd4(const QString &url, const QString &hash)
{
    Q_D(Downloader);
    return d->createDownloadWithHash(url, hash, QCryptographicHash::Md4);
}

QDBusObjectPath Downloader::createDownloadMd5(const QString &url, const QString &hash)
{
    Q_D(Downloader);
    return d->createDownloadWithHash(url, hash, QCryptographicHash::Md5);
}

QDBusObjectPath Downloader::createDownloadSha1(const QString &url, const QString &hash)
{
    Q_D(Downloader);
    return d->createDownloadWithHash(url, hash, QCryptographicHash::Sha1);
}

QDBusObjectPath Downloader::createDownloadSha224(const QString &url, const QString &hash)
{
    Q_D(Downloader);
    return d->createDownloadWithHash(url, hash, QCryptographicHash::Sha224);
}

QDBusObjectPath Downloader::createDownloadSha256(const QString &url, const QString &hash)
{
    Q_D(Downloader);
    return d->createDownloadWithHash(url, hash, QCryptographicHash::Sha256);
}

QDBusObjectPath Downloader::createDownloadSha384(const QString &url, const QString &hash)
{
    Q_D(Downloader);
    return d->createDownloadWithHash(url, hash, QCryptographicHash::Sha384);
}

QDBusObjectPath Downloader::createDownloadSha512(const QString &url, const QString &hash)
{
    Q_D(Downloader);
    return d->createDownloadWithHash(url, hash, QCryptographicHash::Sha512);
}

QList<QDBusObjectPath> Downloader::getAllDownloads()
{
    Q_D(Downloader);
    return d->getAllDownloads();
}

#include "moc_downloader.cpp"

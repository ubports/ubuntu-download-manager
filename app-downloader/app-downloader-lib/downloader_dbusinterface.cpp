#include <QDebug>
#include <QHash>
#include "app_download.h"
#include "application_download_adaptor.h"
#include "downloader.h"
#include "downloader_dbusinterface.h"


/**
 * PRIVATE IMPLEMENATION
 */

class DownloaderDBusInterfacePrivate
{

public:
    explicit DownloaderDBusInterfacePrivate(QDBusConnection connection, DownloaderDBusInterface* parent);

    QDBusObjectPath createDownload(const QString &url);

private:
    QHash<QString, QPair<QSharedPointer<AppDownload>,  ApplicationDownloadAdaptor*> > _downloads;
    QDBusConnection _conn;
    DownloaderDBusInterface* q_ptr;
    Downloader* _downloader;
};

DownloaderDBusInterfacePrivate::DownloaderDBusInterfacePrivate(QDBusConnection connection, DownloaderDBusInterface* parent):
    _conn(connection),
    q_ptr(parent)
{
    _downloader = new Downloader();
}

QDBusObjectPath DownloaderDBusInterfacePrivate::createDownload(const QString &url)
{
    qDebug() << "Creating AppDownload object for " << url;
    QSharedPointer<AppDownload> appDownload = _downloader->getApplication(url);
    ApplicationDownloadAdaptor* adaptor = new ApplicationDownloadAdaptor(appDownload.data());

    // we need to store the ref of both objects, else the mem management will delete them
    _downloads[appDownload->path()] = QPair<QSharedPointer<AppDownload>, ApplicationDownloadAdaptor*>(appDownload, adaptor);
    bool ret = _conn.registerObject(appDownload->path(), appDownload.data());

    qDebug() << "New DBus object registered to " << appDownload->path() << ret;

    return QDBusObjectPath(appDownload->path());
}

/**
 * PUBLIC IMPLEMENTATION
 */

DownloaderDBusInterface::DownloaderDBusInterface(QDBusConnection connection, QObject *parent) :
    QObject(parent),
    d_ptr(new DownloaderDBusInterfacePrivate(connection, this))
{
}


QDBusObjectPath DownloaderDBusInterface::createDownload(const QString &url)
{
    Q_D(DownloaderDBusInterface);
    return d->createDownload(url);
}

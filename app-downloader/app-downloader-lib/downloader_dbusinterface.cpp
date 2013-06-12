#include <QUuid>
#include "downloader_dbusinterface.h"
#include "downloader.h"


/**
 * PRIVATE IMPLEMENATION
 */

class DownloaderDBusInterfacePrivate
{

public:
    explicit DownloaderDBusInterfacePrivate(DownloaderDBusInterface* parent);

    QDBusObjectPath createDownload(const QString &url);

private:
    DownloaderDBusInterface* q_ptr;
    Downloader* _downloader;
};

DownloaderDBusInterfacePrivate::DownloaderDBusInterfacePrivate(DownloaderDBusInterface* parent):
    q_ptr(parent)
{
    _downloader = new Downloader();
}

QDBusObjectPath DownloaderDBusInterfacePrivate::createDownload(const QString &url)
{
    // use a uuid to uniquely identify the downloader
    QUuid uuid = QUuid::createUuid();
    return QDBusObjectPath(uuid.toString());
}

/**
 * PUBLIC IMPLEMENTATION
 */

DownloaderDBusInterface::DownloaderDBusInterface(QObject *parent) :
    QObject(parent),
    d_ptr(new DownloaderDBusInterfacePrivate(this))
{
}


QDBusObjectPath DownloaderDBusInterface::createDownload(const QString &url)
{
    Q_D(DownloaderDBusInterface);
    return d->createDownload(url);
}

#include <QtDBus/QDBusConnection>
#include <QDebug>
#include "downloader_dbusinterface.h"
#include "downloader_adaptor.h"
#include "download_daemon.h"

/**
 * PRIVATE IMPLEMENATION
 */

class DownloadDaemonPrivate
{
    Q_DECLARE_PUBLIC(DownloadDaemon)
public:
    explicit DownloadDaemonPrivate(DownloadDaemon* parent);

    bool start();

private:
    DownloadDaemon* q_ptr;
    QDBusConnection _conn;
    DownloaderDBusInterface* _downInterface;
    DownloaderAdaptor* _downAdaptor;

};

DownloadDaemonPrivate::DownloadDaemonPrivate(DownloadDaemon* parent):
    q_ptr(parent),
    _conn(QDBusConnection::sessionBus())
{
    _downInterface = new DownloaderDBusInterface(q_ptr);
}

bool DownloadDaemonPrivate::start()
{
    qDebug() << "Starting daemon";
    _downAdaptor = new DownloaderAdaptor(_downInterface);
    bool ret = _conn.registerService("com.canonical.Applications.Downloader");
    if (ret)
    {
        qDebug() << "Service registered to com.canonical.Applications.Downloader";
        ret = _conn.registerObject("/", _downAdaptor);
        qDebug() << ret;
        return ret;
    }
    qDebug() << "Could not register";
    return false;
}

/**
 * PUBLIC IMPLEMENTATION
 */

DownloadDaemon::DownloadDaemon(QObject *parent) :
    QObject(parent),
    d_ptr(new DownloadDaemonPrivate(this))
{
}

bool DownloadDaemon::start()
{
    Q_D(DownloadDaemon);
    return d->start();
}

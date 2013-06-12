#ifndef APP_DOWNLOADER_LIB_DOWNLOADER_DBUSINTERFACE_H
#define APP_DOWNLOADER_LIB_DOWNLOADER_DBUSINTERFACE_H

#include <QObject>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusObjectPath>

class DownloaderDBusInterfacePrivate;
class DownloaderDBusInterface : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(DownloaderDBusInterface)
public:
    explicit DownloaderDBusInterface(QDBusConnection connection, QObject *parent = 0);

public slots:
    QDBusObjectPath createDownload(const QString &url);

private:
    DownloaderDBusInterfacePrivate* d_ptr;

};

#endif // DOWNLOADER_DBUSINTERFACE_H

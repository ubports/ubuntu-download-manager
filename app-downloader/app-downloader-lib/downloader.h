#ifndef APP_DOWNLOADER_LIB_DOWNLOADER_H
#define APP_DOWNLOADER_LIB_DOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusObjectPath>
#include "app_download.h"

class DownloaderPrivate;
class Downloader : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Downloader)
public:
    explicit Downloader(QDBusConnection connection, QObject *parent = 0);


public slots:

    QDBusObjectPath createDownload(const QString &url);

private:
    // use pimpl so that we can mantains ABI compatibility
    DownloaderPrivate* d_ptr;
};

#endif // DOWNLOADER_H

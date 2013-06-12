#ifndef APP_DOWNLOADER_LIB_DOWNLOADER_H
#define APP_DOWNLOADER_LIB_DOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QSharedPointer>
#include "app_download.h"

class DownloaderPrivate;
class Downloader : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Downloader)
public:
    explicit Downloader(QObject *parent = 0);

    QSharedPointer<AppDownload> getApplication(QUrl url);
    QSharedPointer<AppDownload> getApplication(QUrl url, QByteArray* hash);

private:
    // use pimpl so that we can mantains ABI compatibility
    DownloaderPrivate* d_ptr;
};

#endif // DOWNLOADER_H

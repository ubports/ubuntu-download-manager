#ifndef APP_DOWNLOADER_LIB_DOWNLOAD_DAEMON_H
#define APP_DOWNLOADER_LIB_DOWNLOAD_DAEMON_H

#include <QObject>
#include "app-downloader-lib_global.h"

class DownloadDaemonPrivate;
class APPDOWNLOADERLIBSHARED_EXPORT DownloadDaemon : public QObject
{
    Q_DECLARE_PRIVATE(DownloadDaemon)
    Q_OBJECT
public:
    explicit DownloadDaemon(QObject *parent = 0);

    bool start();

private:
    // use pimpl so that we can mantains ABI compatibility
    DownloadDaemonPrivate* d_ptr;

};

#endif // DOWNLOAD_DAEMON_H

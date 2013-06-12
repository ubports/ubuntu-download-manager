#ifndef APP_DOWNLOADER_LIB_APP_DOWNLOAD_H
#define APP_DOWNLOADER_LIB_APP_DOWNLOAD_H

#include "app-downloader-lib_global.h"
#include <QObject>
#include <QBuffer>
#include <QNetworkReply>

class AppDownloadPrivate;
class APPDOWNLOADERLIBSHARED_EXPORT AppDownload : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AppDownload)
public:
    explicit AppDownload(QObject* parent=0);

private:
    // private slots used to keep track of the qnetwork reply state

    Q_PRIVATE_SLOT(d_func(), void onDownloadProgress(qint64, qint64))
    Q_PRIVATE_SLOT(d_func(), void onError(QNetworkReply::NetworkError))
    Q_PRIVATE_SLOT(d_func(), void onFinished())
    Q_PRIVATE_SLOT(d_func(), void onFinished(QObject* data))
    Q_PRIVATE_SLOT(d_func(), void onSslErrors(const QList<QSslError>&))

private:
    // use pimpl so that we can mantains ABI compatibility
    AppDownloadPrivate* d_ptr;
};

#endif // APP_DOWNLOAD_H

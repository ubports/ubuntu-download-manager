#include <QBuffer>
#include "app_download.h"



/**
 * PRIVATE IMPLEMENATION
 */

class AppDownloadPrivate
{
    Q_DECLARE_PUBLIC(AppDownload)
public:
    explicit AppDownloadPrivate(AppDownload* parent);

    // slots executed to keep track of the newtork reply
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onError(QNetworkReply::NetworkError code);
    void onFinished();
    void onFinished(QObject* data);
    void onSslErrors(const QList<QSslError>& errors);

private:
    AppDownload* q_ptr;

};

AppDownloadPrivate::AppDownloadPrivate(AppDownload* parent):
    q_ptr(parent)
{
}

void AppDownloadPrivate::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
}

void AppDownloadPrivate::onError(QNetworkReply::NetworkError code)
{
}

void AppDownloadPrivate::onFinished()
{
}

void AppDownloadPrivate::onFinished(QObject* data)
{
    // do cast to QBuffer
    QBuffer* hash = qobject_cast<QBuffer*>(data);
    if (hash != NULL)
    {
    }

    // data downloaded and we do have the hash
    data->deleteLater();
}

void AppDownloadPrivate::onSslErrors(const QList<QSslError>& errors)
{
}

/**
 * PUBLIC IMPLEMENTATION
 */

AppDownload::AppDownload(QObject* parent):
    QObject(parent),
    d_ptr(new AppDownloadPrivate(this))
{
}

#include "moc_app_download.cpp"

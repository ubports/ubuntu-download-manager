#ifndef APP_DOWNLOADER_LIB_NETWORK_REPLY_H
#define APP_DOWNLOADER_LIB_NETWORK_REPLY_H

#include <QByteArray>
#include <QObject>
#include <QNetworkReply>

class NetworkReplyPrivate;
class NetworkReply : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(NetworkReply)

public:
    explicit NetworkReply(QNetworkReply* reply, QObject *parent = 0);
    
public:
    virtual QByteArray readAll();
    virtual void abort();

signals:
    // signals fowarded from the real reply object
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void error(QNetworkReply::NetworkError code);
    void finished();
    void sslErrors(const QList<QSslError>& errors);
    
private:
    Q_PRIVATE_SLOT(d_func(), void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal))
    Q_PRIVATE_SLOT(d_func(), void onError(QNetworkReply::NetworkError code))
    Q_PRIVATE_SLOT(d_func(), void onFinished())
    Q_PRIVATE_SLOT(d_func(), void onSslErrors(const QList<QSslError>& errors))

private:
    // use pimpl so that we can mantains ABI compatibility
    NetworkReplyPrivate* d_ptr;

};

#endif // NETWORK_REPLY_H

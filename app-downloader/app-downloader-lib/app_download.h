#ifndef APP_DOWNLOADER_LIB_APP_DOWNLOAD_H
#define APP_DOWNLOADER_LIB_APP_DOWNLOAD_H

#include "app-downloader-lib_global.h"
#include <QObject>
#include <QByteArray>
#include <QBuffer>
#include <QCryptographicHash>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrl>

class AppDownloadPrivate;
class APPDOWNLOADERLIBSHARED_EXPORT AppDownload : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AppDownload)
public:
    explicit AppDownload(QString path, QUrl url, QNetworkAccessManager* nam, QObject* parent=0);
    explicit AppDownload(QString path, QUrl url, QString hash, QCryptographicHash::Algorithm algo,
        QNetworkAccessManager* nam, QObject* parent=0);

    QString path();
    QUrl url();

public slots:
    void cancel();
    void pause();
    void resume();
    void start();

Q_SIGNALS:
    void canceled(bool success);
    void error(const QString &error);
    void finished();
    void paused(bool success);
    void progress(uint received, uint total);
    void resumed(bool success);
    void started(bool success);

private:
    // private slots used to keep track of the qnetwork reply state

    Q_PRIVATE_SLOT(d_func(), void onDownloadProgress(qint64, qint64))
    Q_PRIVATE_SLOT(d_func(), void onError(QNetworkReply::NetworkError))
    Q_PRIVATE_SLOT(d_func(), void onFinished())
    Q_PRIVATE_SLOT(d_func(), void onSslErrors(const QList<QSslError>&))

private:
    // use pimpl so that we can mantains ABI compatibility
    AppDownloadPrivate* d_ptr;
};

#endif // APP_DOWNLOAD_H

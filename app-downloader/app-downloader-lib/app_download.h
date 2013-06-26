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

    enum State
    {
        IDLE,
        STARTED,
        PAUSED,
        RESUMED,
        CANCELED,
        FINISHED
    };

    explicit AppDownload(QString appId, QString appName, QString path, QUrl url, QNetworkAccessManager* nam, QObject* parent=0);
    explicit AppDownload(QString appId, QString appName, QString path, QUrl url, QString hash, QCryptographicHash::Algorithm algo,
        QNetworkAccessManager* nam, QObject* parent=0);

    // gets for internal state
    QString path();
    QUrl url();
    AppDownload::State state();
    QString filePath();

    // methods that do perform the download
    void cancelDownload();
    void pauseDownload();
    void resumeDownload();
    void startDownload();
    static AppDownload* fromMetadata(const QString &path, QNetworkAccessManager* nam);


public slots:
    // slots that are exposed via dbus, they just change the state, the downloader
    // takes care of the actual download operations
    QString applicationId();
    QString applicationName();
    QVariantMap metadata();
    uint progress();
    uint totalSize();
    void cancel();
    void pause();
    void resume();
    void start();

Q_SIGNALS:
    // signals that are exposed via dbus
    void canceled(bool success);
    void error(const QString &error);
    void finished();
    void paused(bool success);
    void progress(uint received, uint total);
    void resumed(bool success);
    void started(bool success);

    // internal signals used for the download queue
    void stateChanged();

private:
    explicit AppDownload();
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

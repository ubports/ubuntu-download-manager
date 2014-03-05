#ifndef FILE_DOWNLOAD_H
#define FILE_DOWNLOAD_H

#include <QObject>
#include <ubuntu/download_manager/download.h>
#include <ubuntu/download_manager/error.h>
#include <ubuntu/download_manager/manager.h>
#include "download_error.h"

namespace Ubuntu {

namespace DownloadManager {

class SingleDownload : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorChanged)
    Q_PROPERTY(bool isCompleted READ isCompleted NOTIFY isCompletedChanged)
    Q_PROPERTY(bool downloadInProgress READ downloadInProgress NOTIFY downloadInProgressChanged)
    Q_PROPERTY(bool allowMobileDownload READ allowMobileDownload WRITE setAllowMobileDownload NOTIFY allowMobileDownloadChanged)
    Q_PROPERTY(qulonglong throttle READ throttle WRITE setThrottle NOTIFY throttleChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool downloading READ downloading NOTIFY downloadingChanged)

public:
    explicit SingleDownload(QObject *parent = 0);

    Q_INVOKABLE void pause();
    Q_INVOKABLE void resume();
    Q_INVOKABLE void cancel();
    Q_INVOKABLE void download(QString url);

    void start();
    bool isCompleted() { return m_completed; }
    DownloadError& error() { return m_error; }
    QString errorMessage() { return m_error.message(); }
    bool allowMobileDownload() { return m_download->isMobileDownloadAllowed(); }
    void setAllowMobileDownload(bool value) { m_download->allowMobileDownload(value); emit allowMobileDownloadChanged(); }
    qulonglong throttle() { return m_download->throttle(); }
    void setThrottle(qulonglong value) { m_download->setThrottle(value); emit throttleChanged(); }
    int progress() { return m_progress; }
    bool downloading() { return m_downloading; }
    bool downloadInProgress() { return m_downloadInProgress; }

signals:
    void isCompletedChanged();
    void allowMobileDownloadChanged();
    void throttleChanged();
    void progressChanged();
    void downloadingChanged();
    void downloadInProgressChanged();

    void canceled(bool success);
    void finished(const QString& path);
    void paused(bool success);
    void processing(const QString &path);
    void progressReceived(qulonglong received, qulonglong total);
    void resumed(bool success);
    void started(bool success);
    void errorFound(DownloadError& error);
    void errorChanged();

public slots:
    void registerError(Error* error);
    void setCompleted();
    void bindDownload(Download* download);
    void setProgress(qulonglong received, qulonglong total);
    void setDownloadPaused(bool);
    void setDownloadStarted(bool);
    void setDownloadCanceled(bool);

private:
    bool m_completed;
    bool m_downloading;
    bool m_downloadInProgress;
    int m_progress;
    DownloadError m_error;
    Download* m_download;
    Manager* m_manager;

};

}
}

#endif // FILE_DOWNLOAD_H

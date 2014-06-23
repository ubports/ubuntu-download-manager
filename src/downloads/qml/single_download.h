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
    Q_PROPERTY(bool autoStart READ autoStart WRITE setAutoStart)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorChanged)
    Q_PROPERTY(bool isCompleted READ isCompleted NOTIFY isCompletedChanged)
    Q_PROPERTY(bool downloadInProgress READ downloadInProgress NOTIFY downloadInProgressChanged)
    Q_PROPERTY(bool allowMobileDownload READ allowMobileDownload WRITE setAllowMobileDownload NOTIFY allowMobileDownloadChanged)
    Q_PROPERTY(qulonglong throttle READ throttle WRITE setThrottle NOTIFY throttleChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool downloading READ downloading NOTIFY downloadingChanged)
    Q_PROPERTY(QString downloadId READ downloadId NOTIFY downloadIdChanged)
    Q_PROPERTY(QVariantMap headers READ headers WRITE setHeaders NOTIFY headersChanged)

public:
    explicit SingleDownload(QObject *parent = 0);

    Q_INVOKABLE void start();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void resume();
    Q_INVOKABLE void cancel();
    Q_INVOKABLE void download(QString url);

    void startDownload();

    // getters
    bool isCompleted() const {
        return m_completed;
    }

    DownloadError& error() {
        return m_error;
    }

    QString errorMessage() const {
        return m_error.message();
    }

    qulonglong throttle() const {
        return m_download->throttle();
    }

    bool allowMobileDownload() const {
        return m_download->isMobileDownloadAllowed();
    }

    int progress() const {
        return m_progress;
    }

    bool downloading() const {
        return m_downloading;
    }

    bool downloadInProgress() const {
        return m_downloadInProgress;
    }

    bool autoStart() const {
        return m_autoStart;
    }

    QString downloadId() const {
        return m_download->id();
    }

    QVariantMap headers() const {
        // convert the QMap<QString, QString> into a QMap<QString, QVariant>
        auto headers = m_download->headers();
        QVariantMap result;
        foreach(const QString& key, headers.keys()) {
            result[key] = headers[key]; // automatic conversion
        }
        return result;
    }

    // setters
    void setAllowMobileDownload(bool value) {
        m_download->allowMobileDownload(value);
        if (m_download->isError()) {
            // set the error details and emit the signals
            auto err = m_download->error();
            m_error.setType(getErrorType(err->type()));
            m_error.setMessage(err->errorString());
            emit errorFound(m_error);
            emit errorChanged();
        } else {
            emit allowMobileDownloadChanged();
        }
    }

    void setThrottle(qulonglong value) {
        m_download->setThrottle(value);
        if (m_download->isError()) {
            // set the error details and emit the signals
            auto err = m_download->error();
            m_error.setType(getErrorType(err->type()));
            m_error.setMessage(err->errorString());
            emit errorFound(m_error);
            emit errorChanged();
        } else {
            emit throttleChanged();
        }
    }

    void setAutoStart(bool value) {
        m_autoStart = value;
    }

    void setHeaders(QVariantMap headers) {
        QMap<QString, QString> stringMap;
        // convert the QVariantMap in a QMap<QString, QString> and make sure
        // that the variants can be converted to strings.
        foreach(const QString& key, headers.keys()) {
            auto data = headers[key];
            if (data.canConvert<QString>()) {
                stringMap[key] = data.toString();
            } else {
                m_error.setType("Headers Conversion Error");
                auto msg = QString(
                    "Could not convert data in header '%1' to string.").arg(key);
                m_error.setMessage(msg);
                emit errorFound(m_error);
                emit errorChanged();
                return;
            }
        }

        m_download->setHeaders(stringMap);
        if (m_download->isError()) {
            // set the error details and emit the signals
            auto err = m_download->error();
            m_error.setType(getErrorType(err->type()));
            m_error.setMessage(err->errorString());
            emit errorFound(m_error);
            emit errorChanged();
        } else {
            emit headersChanged();
        }
    }

signals:
    void isCompletedChanged();
    void allowMobileDownloadChanged();
    void throttleChanged();
    void progressChanged();
    void downloadingChanged();
    void downloadInProgressChanged();
    void downloadIdChanged();
    void headersChanged();

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
    QString getErrorType(Error::Type type) {
        switch (type) {
            case Error::Auth:
                return QString("Auth");
            case Error::DBus:
                return QString("DBus");
            case Error::Http:
                return QString("Http");
            case Error::Network:
                return QString("Network");
            case Error::Process:
                return QString("Process");
            default:
                return QString();
        }
    }

private:
    bool m_autoStart;
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

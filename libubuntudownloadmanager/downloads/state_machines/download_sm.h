/*
 * Copyright 2013 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef DOWNLOADER_LIB_DOWNLOAD_SM_H
#define DOWNLOADER_LIB_DOWNLOAD_SM_H

#include <QSignalTransition>
#include <QAbstractState>
#include <QState>
#include <QObject>
#include <QVariant>
#include "downloads/sm_file_download.h"

namespace Ubuntu {

namespace DownloadManager {

namespace StateMachines {

class DownloadSMTransition : public QSignalTransition {
    Q_OBJECT

 public:
    DownloadSMTransition(const SMFileDownload* sender,
                         const char* signal,
                         QState* sourceState,
                         QAbstractState* next)
        : QSignalTransition(sender, signal, sourceState) {
        setTargetState(next);
    }
 protected:
    SMFileDownload* download() {
        return qobject_cast<SMFileDownload*>(senderObject());
    }
};

// takes care of the case in which the header is correctly performed
class HeaderTransition : public DownloadSMTransition  {
    Q_OBJECT

 public:
    HeaderTransition(const SMFileDownload* sender,
                     QState* sourceState,
                     QAbstractState* nextState);
 protected:
    virtual void onTransition(QEvent * event) override;
};

// takes care of the case where there was an error during a network request
class NetworkErrorTransition : public DownloadSMTransition {
    Q_OBJECT
 public:
    NetworkErrorTransition(const SMFileDownload* sender,
                           QState* sourceState,
                           QAbstractState* nextState);
 protected:
    virtual void onTransition(QEvent * event) override;
};

// takes care of the case where there was an ssl error
class SslErrorTransition : public DownloadSMTransition {
    Q_OBJECT
 public:
    SslErrorTransition(const SMFileDownload* sender,
                       QState* sourceState,
                       QAbstractState* nextState);
 protected:
    virtual void onTransition(QEvent * event) override;
};

// takes care of the very first time the download is started
class StartDownloadTransition : public DownloadSMTransition {
    Q_OBJECT
 public:
    StartDownloadTransition(const SMFileDownload* sender,
                            QState* sourceState,
                            QAbstractState* nextState);
 protected:
    virtual void onTransition(QEvent * event) override;
};

// takes care of the case in witch the connection is lost
class StopRequestTransition : public DownloadSMTransition {
    Q_OBJECT
 public:
    StopRequestTransition(const SMFileDownload* sender,
                          const char* signal,
                          QState* sourceState,
                          QAbstractState* nextState);
 protected:
    virtual void onTransition(QEvent * event) override;
};

// takes care of the case when the download is canceled
class CancelDownloadTransition : public DownloadSMTransition {
    Q_OBJECT
 public:
    CancelDownloadTransition(const SMFileDownload* sender,
                             QState* sourceState,
                             QAbstractState* nextState);
 protected:
    virtual void onTransition(QEvent * event) override;
};

class ResumeDownloadTransition : public DownloadSMTransition {
    Q_OBJECT
 public:
    ResumeDownloadTransition(const SMFileDownload* sender,
                             const char* signal,
                             QState* sourceState,
                             QAbstractState* nextState);
 protected:
    virtual void onTransition(QEvent * event) override;
};

class DownloadSMPrivate;
class DownloadSM : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(DownloadSM)

    Q_PROPERTY(QString state READ state WRITE setState)

 public:
    DownloadSM(SMFileDownload* down, QObject *parent = 0);
    virtual ~DownloadSM();
    
    QString state();
    void setState(QString state);

    void start();

    static QString IDLE;
    static QString INIT;
    static QString DOWNLOADING;
    static QString DOWNLOADING_NOT_CONNECTED;
    static QString PAUSED;
    static QString PAUSED_NOT_CONNECTED;
    static QString DOWNLOADED;
    static QString HASHING;
    static QString POST_PROCESSING;
    static QString ERROR;
    static QString CANCELED;
    static QString FINISHED;

 signals:
    void started();
    void stopped();
    void finished();
    void stateChanged(QString);

 private:
    // use pimpl so that we can mantains ABI compatibility
    DownloadSMPrivate* d_ptr;
};

}  // StateMachines

}  // DownloadManager

}  // Ubuntu

#endif // DOWNLOAD_SM_H

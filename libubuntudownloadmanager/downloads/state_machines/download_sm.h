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
#include <QState>
#include <QObject>
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
                         QState* next)
        : QSignalTransition(sender, signal, sourceState) {
        setTargetState(next);
    }
 protected:
    SMFileDownload* download() {
        return qobject_cast<SMFileDownload*>(senderObject());
    }
};

class DownloadSMPrivate;
class DownloadSM : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(DownloadSM)

 public:
    explicit DownloadSM(QObject *parent = 0);
    
 signals:
    
 public slots:
    
 private:
    // use pimpl so that we can mantains ABI compatibility
    DownloadSMPrivate* d_ptr;
};

}  // StateMachines

}  // DownloadManager

}  // Ubuntu

#endif // DOWNLOAD_SM_H

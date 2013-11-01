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

#include <QFinalState>
#include <QState>
#include <QStateMachine>
#include <QSharedPointer>
#include "download_sm.h"

namespace Ubuntu {

namespace DownloadManager {

namespace StateMachines {

/**
 * PRIVATE IMPLEMENATION
 */

class DownloadSMPrivate {
    Q_DECLARE_PUBLIC(DownloadSM)

 public:
    explicit DownloadSMPrivate(DownloadSM* parent)
        : q_ptr(parent) {

        // create the diff events and add them to the state machine
        _idle = QSharedPointer<QState>(new QState());
        _init = QSharedPointer<QState>(new QState());
        _downloading = QSharedPointer<QState>(new QState());
        _downloadingNotConnected = QSharedPointer<QState>(new QState());
        _paused = QSharedPointer<QState>(new QState());
        _pausedNotConnected = QSharedPointer<QState>(new QState());
        _downloaded = QSharedPointer<QState>(new QState());
        _hashing = QSharedPointer<QState>(new QState());
        _postProcessing = QSharedPointer<QState>(new QState());

        // finish steps
        _error = QSharedPointer<QFinalState>(new QFinalState());
        _canceled = QSharedPointer<QFinalState>(new QFinalState());
        _finished = QSharedPointer<QFinalState>(new QFinalState());
    }

 private:
    QStateMachine _stateMachine;

    // intermediate steps
    QSharedPointer<QState>_idle;
    QSharedPointer<QState>_init;
    QSharedPointer<QState>_downloading;
    QSharedPointer<QState>_downloadingNotConnected;
    QSharedPointer<QState>_paused;
    QSharedPointer<QState>_pausedNotConnected;
    QSharedPointer<QState>_downloaded;
    QSharedPointer<QState>_hashing;
    QSharedPointer<QState>_postProcessing;
    // finish steps
    QSharedPointer<QFinalState>_error;
    QSharedPointer<QFinalState>_canceled;
    QSharedPointer<QFinalState>_finished;

    DownloadSM* q_ptr;
};

DownloadSM::DownloadSM(QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadSMPrivate(this)){
}

}  // StateMachines

}  // DownloadManager

}  // Ubuntu

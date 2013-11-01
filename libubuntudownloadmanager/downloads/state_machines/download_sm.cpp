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
        _idle = new QState();
        _init = new QState();
        _downloading = new QState();
        _downloadingNotConnected = new QState();
        _paused = new QState();
        _pausedNotConnected = new QState();
        _downloaded = new QState();
        _hashing = new QState();
        _postProcessing = new QState();

        // finish steps
        _error = new QFinalState();
        _canceled = new QFinalState();
        _finished = new QFinalState();
    }

 private:
    QStateMachine _stateMachine;

    // intermediate steps
    QState* _idle;
    QState* _init;
    QState* _downloading;
    QState* _downloadingNotConnected;
    QState* _paused;
    QState* _pausedNotConnected;
    QState* _downloaded;
    QState* _hashing;
    QState* _postProcessing;
    // finish steps
    QFinalState* _error;
    QFinalState* _canceled;
    QFinalState* _finished;

    DownloadSM* q_ptr;
};

DownloadSM::DownloadSM(QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadSMPrivate(this)){
}

}  // StateMachines

}  // DownloadManager

}  // Ubuntu

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
#include <QScopedPointer>
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
        : _idle(new QState()),
          _init(new QState()),
          _downloading(new QState()),
          _downloadingNotConnected(new QState()),
          _paused(new QState()),
          _pausedNotConnected(new QState()),
          _downloaded(new QState()),
          _hashing(new QState()),
          _postProcessing(new QState()),
          _error(new QFinalState()),
          _canceled(new QFinalState()),
          _finished(new QFinalState()),
          q_ptr(parent) {
    }

 private:
    QStateMachine _stateMachine;

    // intermediate steps
    QScopedPointer<QState>_idle;
    QScopedPointer<QState>_init;
    QScopedPointer<QState>_downloading;
    QScopedPointer<QState>_downloadingNotConnected;
    QScopedPointer<QState>_paused;
    QScopedPointer<QState>_pausedNotConnected;
    QScopedPointer<QState>_downloaded;
    QScopedPointer<QState>_hashing;
    QScopedPointer<QState>_postProcessing;
    // finish steps
    QScopedPointer<QFinalState>_error;
    QScopedPointer<QFinalState>_canceled;
    QScopedPointer<QFinalState>_finished;

    DownloadSM* q_ptr;
};

DownloadSM::DownloadSM(QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadSMPrivate(this)){
}

}  // StateMachines

}  // DownloadManager

}  // Ubuntu

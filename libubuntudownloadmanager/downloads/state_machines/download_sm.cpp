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
#define IDLE_STATE 0
#define INIT_STATE 1
#define DOWNLOADING_STATE 2
#define DOWNLOADING_NOT_CONNECTED_STATE  3
#define PAUSED_STATE 4
#define PAUSED_NOT_CONNECTED_STATE 5
#define DOWNLOADED_STATE 6
#define HASHING_STATE 7
#define POST_PROCESSING_STATE 8
#define ERROR_STATE 9
#define CANCELED_STATE 10
#define FINISHED_STATE 11

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
        _states.append(new QState());
        _states.append(new QState());
        _states.append(new QState());
        _states.append(new QState());
        _states.append(new QState());
        _states.append(new QState());
        _states.append(new QState());
        _states.append(new QState());
        _states.append(new QState());
        _states.append(new QFinalState());
        _states.append(new QFinalState());
        _states.append(new QFinalState());
    }

    ~DownloadSMPrivate() {
        qDeleteAll(_states);
    }

 private:
    QStateMachine _stateMachine;
    QList<QAbstractState*> _states;

    DownloadSM* q_ptr;
};

DownloadSM::DownloadSM(QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadSMPrivate(this)){
}

DownloadSM::~DownloadSM() {
    delete d_ptr;
}

}  // StateMachines

}  // DownloadManager

}  // Ubuntu

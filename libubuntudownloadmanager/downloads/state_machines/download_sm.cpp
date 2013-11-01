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
#include <QSslError>
#include <QSharedPointer>
#include "download_sm.h"

namespace Ubuntu {

namespace DownloadManager {

namespace StateMachines {


/*
 * HEAD TRANSITION
 */
HeaderTransition::HeaderTransition(const SMFileDownload* sender,
                                   QState* sourceState,
                                   QAbstractState* nextState)
    : DownloadSMTransition(sender, SIGNAL(headRequestCompleted()),
        sourceState, nextState) {
}

void
HeaderTransition::onTransition(QEvent * event) {
    DownloadSMTransition::onTransition(event);
}

/*
 * NETWORK ERROR TRANSITION
 */

NetworkErrorTransition::NetworkErrorTransition(const SMFileDownload* sender,
                                               QState* sourceState,
                                               QAbstractState* nextState)
    : DownloadSMTransition(sender, SIGNAL(error(QNetworkReply::NetworkError)),
        sourceState, nextState) {
}

void
NetworkErrorTransition::onTransition(QEvent* event) {
    SMFileDownload* down = download();
    QStateMachine::SignalEvent* e =
        static_cast<QStateMachine::SignalEvent*>(event);
    QVariant v = e->arguments().at(0);
    QNetworkReply::NetworkError code = v.value<QNetworkReply::NetworkError>();

    down->emitNetworkError(code);
    DownloadSMTransition::onTransition(event);
}

/*
 * SSL ERROR TRANSITION
 */

SslErrorTransition::SslErrorTransition(const SMFileDownload* sender,
                                       QState* sourceState,
                                       QAbstractState* nextState)
    : DownloadSMTransition(sender, SIGNAL(sslErrors(const QList<QSslError>&)),
        sourceState, nextState) {
}

void
SslErrorTransition::onTransition(QEvent * event) {
    SMFileDownload* down = download();
    QStateMachine::SignalEvent* e =
        static_cast<QStateMachine::SignalEvent*>(event);
    QVariant v = e->arguments().at(0);
    QList<QSslError> errors = v.value<QList<QSslError> > ();

    down->emitSslError(errors);
    DownloadSMTransition::onTransition(event);
}

/*
 * START TRANSITION
 */

StartDownloadTransition::StartDownloadTransition(const SMFileDownload* sender,
                                                 QState* sourceState,
                                                 QAbstractState* nextState)
    : DownloadSMTransition(sender, SIGNAL(downloadingStarted()),
        sourceState, nextState) {
}

void
StartDownloadTransition::onTransition(QEvent * event) {
    Q_UNUSED(event);
    SMFileDownload* down = download();
    // tell the down to start and set the state
    down->requestDownload();
    down->setState(Download::START);
}

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

        // add the idle state transitions
        _headerTransition = QSharedPointer<HeaderTransition>(
            new HeaderTransition(_down, _idle.data(), _init.data()));
        _idleNetworkErrorTransition = QSharedPointer<NetworkErrorTransition>(
            new NetworkErrorTransition(_down, _idle.data(), _error.data()));
        _idleSslErrorTransition = QSharedPointer<SslErrorTransition>(
            new SslErrorTransition(_down, _idle.data(), _error.data()));
        _idle->addTransition(_headerTransition.data());
        _idle->addTransition(_idleNetworkErrorTransition.data());
        _idle->addTransition(_idleSslErrorTransition.data());

        // add the init state transtions
        _startDownload = new StartDownloadTransition(_down, _init.data(), _downloading.data());
        _initNetworkErrorTransition = new NetworkErrorTransition(_down, _init.data(), _error.data());
        _initSslErrorTransition = new SslErrorTransition(_down, _init.data(), _error.data());
        _init->addTransition(_startDownload);
        _init->addTransition(_initNetworkErrorTransition);
        _init->addTransition(_initSslErrorTransition);
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
    // idle transitions
    QSharedPointer<HeaderTransition> _headerTransition;
    QSharedPointer<NetworkErrorTransition> _idleNetworkErrorTransition;
    QSharedPointer<SslErrorTransition> _idleSslErrorTransition;
    // init transitions
    QSharedPointer<StartDownloadTransition> _startDownload;
    QSharedPointer<NetworkErrorTransition> _initNetworkErrorTransition;
    QSharedPointer<SslErrorTransition> _initSslErrorTransition;

    SMFileDownload* _down;
    DownloadSM* q_ptr;
};

DownloadSM::DownloadSM(QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadSMPrivate(this)){
}

}  // StateMachines

}  // DownloadManager

}  // Ubuntu

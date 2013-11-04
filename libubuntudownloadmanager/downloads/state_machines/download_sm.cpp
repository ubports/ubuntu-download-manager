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

#define ERROR_STATE 0
#define CANCELED_STATE 1
#define FINISHED_STATE 2

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

/*
 * DOWNLOADING LOST CONNECTION
 */

StopRequestTransition::StopRequestTransition(const SMFileDownload* sender,
                                             const char* signal,
                                             QState* sourceState,
                                             QAbstractState* nextState)
    : DownloadSMTransition(sender, signal, sourceState, nextState){
}

void
StopRequestTransition::onTransition(QEvent * event) {
    Q_UNUSED(event);
    SMFileDownload* down = download();
    down->stopRequestDownload();
    down->setState(Download::PAUSE);
}

/*
 * CANCEL DOWNLOAD TRANSITION
 */

CancelDownloadTransition::CancelDownloadTransition(const SMFileDownload* sender,
                                                   QState* sourceState,
                                                   QAbstractState* nextState)
    : DownloadSMTransition(sender, SIGNAL(canceled()), sourceState, nextState) {
}

void
CancelDownloadTransition::onTransition(QEvent * event) {
    Q_UNUSED(event);
    SMFileDownload* down = download();
    down->cancelRequestDownload();
    down->setState(Download::CANCEL);
}

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
        _finalStates.append(new QFinalState());
        _finalStates.append(new QFinalState());
        _finalStates.append(new QFinalState());

        // add the idle state transitions
        _transitions.append(new HeaderTransition(_down,
            _states[IDLE_STATE], _states[INIT_STATE]));
        _states[IDLE_STATE]->addTransition(_transitions.last());

        _transitions.append(new NetworkErrorTransition(
            _down, _states[IDLE_STATE], _finalStates[ERROR_STATE]));
        _states[IDLE_STATE]->addTransition(_transitions.last());

        _transitions.append(new SslErrorTransition(_down,
            _states[IDLE_STATE], _finalStates[ERROR_STATE]));
        _states[IDLE_STATE]->addTransition(_transitions.last());

        // add the init state transtions
        _transitions.append(new StartDownloadTransition(_down,
            _states[INIT_STATE], _states[DOWNLOADING_STATE]));
        _states[INIT_STATE]->addTransition(_transitions.last());

        _transitions.append(new NetworkErrorTransition(_down,
            _states[INIT_STATE], _finalStates[ERROR_STATE]));
        _states[INIT_STATE]->addTransition(_transitions.last());

        _transitions.append(new SslErrorTransition(_down,
            _states[INIT_STATE], _finalStates[ERROR_STATE]));
        _states[INIT_STATE]->addTransition(_transitions.last());

        // add the downloading transitions
        _transitions.append(new StopRequestTransition(_down,
            SIGNAL(connectionDisabled()), _states[DOWNLOADING_STATE],
            _states[DOWNLOADING_NOT_CONNECTED_STATE]));
        _states[DOWNLOADING_STATE]->addTransition(_transitions.last());

        _transitions.append(new StopRequestTransition(_down,
            SIGNAL(paused()), _states[DOWNLOADING_STATE], _states[PAUSED_STATE]));
        _states[DOWNLOADING_STATE]->addTransition(_transitions.last());

        _transitions.append(new CancelDownloadTransition(_down,
            _states[DOWNLOADING_STATE], _finalStates[CANCELED_STATE]));
        _states[DOWNLOADING_STATE]->addTransition(_transitions.last());

        _transitions.append(new NetworkErrorTransition(_down,
            _states[DOWNLOADING_STATE], _finalStates[ERROR_STATE]));
        _states[DOWNLOADING_STATE]->addTransition(_transitions.last());

        _transitions.append(new SslErrorTransition(_down,
            _states[DOWNLOADING_STATE], _finalStates[ERROR_STATE]));
        _states[DOWNLOADING_STATE]->addTransition(_transitions.last());
    }

    ~DownloadSMPrivate() {
        qDeleteAll(_transitions);
        qDeleteAll(_states);
        qDeleteAll(_finalStates);
    }

 private:
    QStateMachine _stateMachine;
    QList<QState*> _states;
    QList<QFinalState*> _finalStates;
    QList<QSignalTransition*> _transitions;

    SMFileDownload* _down;
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

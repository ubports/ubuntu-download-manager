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

#include <QDebug>
#include <QState>
#include <QStateMachine>
#include <QSslError>
#include "system/logger.h"
#include "download_sm.h"
#include "final_state.h"

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
    TRACE << event;
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
    TRACE << event;
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
    TRACE << event;
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
    TRACE << event;
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
    TRACE << event;
    SMFileDownload* down = download();
    down->cancelRequestDownload();
    down->setState(Download::CANCEL);
}

/*
 * RESUME DOWNLOAD TRANSITION
 */
ResumeDownloadTransition::ResumeDownloadTransition(const SMFileDownload* sender,
                                                   const char* signal,
                                                   QState* sourceState,
                                                   QAbstractState* nextState)
    : DownloadSMTransition(sender, signal, sourceState, nextState) {
}

void
ResumeDownloadTransition::onTransition(QEvent * event) {
    TRACE << event;
    SMFileDownload* down = download();
    down->requestDownload();
    down->setState(Download::RESUME);
}

/**
 * PRIVATE IMPLEMENATION
 */

class DownloadSMPrivate {
    Q_DECLARE_PUBLIC(DownloadSM)

 public:
    explicit DownloadSMPrivate(SMFileDownload* down, DownloadSM* parent)
        : _down(down),
          q_ptr(parent) {
        Q_Q(DownloadSM);

        _states.append(new QState());
        _states[IDLE_STATE]->assignProperty(q, "state", "IDLE");
        _states.append(new QState());
        _states[INIT_STATE]->assignProperty(q, "state", "INIT");
        _states.append(new QState());
        _states[DOWNLOADING_STATE]->assignProperty(q, "state",
            "DOWNLOADING");
        _states.append(new QState());
        _states[DOWNLOADING_NOT_CONNECTED_STATE]->assignProperty(q, "state",
            "DOWNLOADING_NOT_CONNECTED");
        _states.append(new QState());
        _states[PAUSED_STATE]->assignProperty(q, "state", "PAUSED");
        _states.append(new QState());
        _states[PAUSED_NOT_CONNECTED_STATE]->assignProperty(q, "state",
            "PAUSED_NOT_CONNECTED");
        _states.append(new QState());
        _states[DOWNLOADED_STATE]->assignProperty(q, "state",
            "DOWNLOADED");
        _states.append(new QState());
        _states[HASHING_STATE]->assignProperty(q, "state", "HASHING");
        _states.append(new QState());
        _states[POST_PROCESSING_STATE]->assignProperty(q, "state",
            "POST_PROCESSING");
        _finalStates.append(new FinalState());
        _finalStates[ERROR_STATE]->assignProperty(q, "state",
            "ERROR");
        _finalStates.append(new FinalState());
        _finalStates[CANCELED_STATE]->assignProperty(q, "state",
            "CANCELED");
        _finalStates.append(new FinalState());
        _finalStates[FINISHED_STATE]->assignProperty(q, "state",
            "FINISHED");

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

        // add the downloading not connected transitions
        _transitions.append(new ResumeDownloadTransition(_down,
            SIGNAL(connectionEnabled()),
            _states[DOWNLOADING_NOT_CONNECTED_STATE],
            _states[DOWNLOADING_STATE]));
        _states[DOWNLOADING_NOT_CONNECTED_STATE]->addTransition(_transitions.last());

        _transitions.append(new CancelDownloadTransition(_down,
            _states[DOWNLOADING_NOT_CONNECTED_STATE], _finalStates[CANCELED_STATE]));
        _states[DOWNLOADING_NOT_CONNECTED_STATE]->addTransition(_transitions.last());

        _transitions.append(_states[DOWNLOADING_NOT_CONNECTED_STATE]->addTransition(_down,
            SIGNAL(paused()), _states[PAUSED_NOT_CONNECTED_STATE]));

        // add the pause transitions
        _transitions.append(new ResumeDownloadTransition(_down,
            SIGNAL(resumed()), _states[PAUSED_STATE], _states[DOWNLOADING_STATE]));
        _states[PAUSED_STATE]->addTransition(_transitions.last());

        _transitions.append(new CancelDownloadTransition(_down,
            _states[PAUSED_STATE], _finalStates[CANCELED_STATE]));
        _states[PAUSED_STATE]->addTransition(_transitions.last());

        _transitions.append(_states[PAUSED_STATE]->addTransition(
            _down, SIGNAL(connectionDisabled()), _states[PAUSED_NOT_CONNECTED_STATE]));

        // paused not connected transitions
        _transitions.append(new CancelDownloadTransition(
            _down, _states[PAUSED_NOT_CONNECTED_STATE], _finalStates[CANCELED_STATE]));
        _states[PAUSED_NOT_CONNECTED_STATE]->addTransition(_transitions.last());

        _transitions.append(_states[PAUSED_NOT_CONNECTED_STATE]->addTransition(
            _down, SIGNAL(resumed()), _states[DOWNLOADING_NOT_CONNECTED_STATE]));

        _transitions.append(_states[PAUSED_NOT_CONNECTED_STATE]->addTransition(_down,
            SIGNAL(connectionEnabled()), _states[PAUSED_STATE]));

        // downloaded transitions
        _transitions.append(new CancelDownloadTransition(
            _down, _states[DOWNLOADED_STATE], _finalStates[CANCELED_STATE]));
        _states[DOWNLOADED_STATE]->addTransition(_transitions.last());

        _transitions.append(_states[DOWNLOADED_STATE]->addTransition(_down,
            SIGNAL(finished()), _finalStates[FINISHED_STATE]));

        _transitions.append(_states[DOWNLOADED_STATE]->addTransition(_down,
            SIGNAL(hashingStarted()), _states[HASHING_STATE]));

        _transitions.append(_states[DOWNLOADED_STATE]->addTransition(_down,
            SIGNAL(postProcessingStarted()), _states[POST_PROCESSING_STATE]));

        // hashing transitions
        _transitions.append(_states[HASHING_STATE]->addTransition(_down,
            SIGNAL(finished()), _finalStates[FINISHED_STATE]));
        _transitions.append(_states[HASHING_STATE]->addTransition(_down,
            SIGNAL(hashingError()), _finalStates[ERROR_STATE]));
        _transitions.append(_states[HASHING_STATE]->addTransition(_down,
            SIGNAL(postProcessingStarted()), _states[POST_PROCESSING_STATE]));

        // post processing transitions
        _transitions.append(_states[POST_PROCESSING_STATE]->addTransition(
            _down, SIGNAL(finished()), _finalStates[FINISHED_STATE]));
        _transitions.append(_states[POST_PROCESSING_STATE]->addTransition(
            _down, SIGNAL(postProcessingError()), _finalStates[ERROR_STATE]));

        // add states set init state
        foreach(QState* state, _states) {
            _stateMachine.addState(state);
        }
        foreach(FinalState* state, _finalStates) {
            _stateMachine.addState(state);
        }

        _stateMachine.setInitialState(_states[IDLE_STATE]);

        // connect the signals
        q->connect(&_stateMachine, SIGNAL(started()),
            q, SIGNAL(started()));
        q->connect(&_stateMachine, SIGNAL(stopped()),
            q, SIGNAL(stopped()));
        q->connect(&_stateMachine, SIGNAL(finished()),
            q, SIGNAL(finished()));
    }

    QString state() {
        return _state;
    }

    void setState(QString state) {
        Q_Q(DownloadSM);
        _state = state;
        qDebug() << _state;
        emit q->stateChanged(state);
    }

    void start() {
        _stateMachine.start();
    }

    ~DownloadSMPrivate() {
        qDeleteAll(_transitions);
        qDeleteAll(_states);
        qDeleteAll(_finalStates);
    }

 private:
    QString _state = "IDLE";
    QStateMachine _stateMachine;
    QList<QState*> _states;
    QList<FinalState*> _finalStates;
    QList<QSignalTransition*> _transitions;

    SMFileDownload* _down;
    DownloadSM* q_ptr;
};

DownloadSM::DownloadSM(SMFileDownload* down, QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadSMPrivate(down, this)){
}

QString
DownloadSM::state() {
    Q_D(DownloadSM);
    return d->state();
}

void
DownloadSM::setState(QString state) {
    Q_D(DownloadSM);
    d->setState(state);
}

void
DownloadSM::start() {
    Q_D(DownloadSM);
    d->start();
}

DownloadSM::~DownloadSM() {
    delete d_ptr;
}

}  // StateMachines

}  // DownloadManager

}  // Ubuntu

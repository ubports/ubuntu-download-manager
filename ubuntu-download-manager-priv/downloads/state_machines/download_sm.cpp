/*
 * Copyright 2013-2014 Canonical Ltd.
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
#include "system/logger.h"
#include "download_sm.h"
#include "final_state.h"
#include "state.h"

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

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
    DLOG(INFO) << " " << __PRETTY_FUNCTION__;
    SMFileDownload* down = download();
    QStateMachine::SignalEvent* e =
        static_cast<QStateMachine::SignalEvent*>(event);
    if (e->arguments().count() > 0) {
        QVariant v = e->arguments().at(0);
        QNetworkReply::NetworkError code = v.value<QNetworkReply::NetworkError>();
        down->emitNetworkError(code);
    } else {
        LOG(ERROR) << "Signal does not have events!";
    }
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
    DLOG(INFO) << " " << __PRETTY_FUNCTION__;
    SMFileDownload* down = download();
    QStateMachine::SignalEvent* e =
        static_cast<QStateMachine::SignalEvent*>(event);
    if (e->arguments().count() > 0) {
        QVariant v = e->arguments().at(0);
        QList<QSslError> errors = v.value<QList<QSslError> > ();
        down->emitSslError(errors);
    } else {
        LOG(ERROR) << "Signal does not have events!";
    }
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
    DLOG(INFO) << " " << __PRETTY_FUNCTION__;
    SMFileDownload* down = download();
    // tell the down to start and set the state
    down->requestDownload();
    down->setState(Download::START);
    DownloadSMTransition::onTransition(event);
}

/*
 * DOWNLOADING LOST CONNECTION
 */

PauseRequestTransition::PauseRequestTransition(const SMFileDownload* sender,
                                             const char* signal,
                                             QState* sourceState,
                                             QAbstractState* nextState)
    : DownloadSMTransition(sender, signal, sourceState, nextState){
}

void
PauseRequestTransition::onTransition(QEvent * event) {
    DLOG(INFO) << " " << __PRETTY_FUNCTION__;
    SMFileDownload* down = download();
    down->pauseRequestDownload();
    down->setState(Download::PAUSE);
    DownloadSMTransition::onTransition(event);
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
    DLOG(INFO) << " " << __PRETTY_FUNCTION__;
    SMFileDownload* down = download();
    down->cancelRequestDownload();
    down->setState(Download::CANCEL);
    DownloadSMTransition::onTransition(event);
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
    DLOG(INFO) << " " << __PRETTY_FUNCTION__;
    SMFileDownload* down = download();
    down->requestDownload();
    down->setState(Download::RESUME);
    DownloadSMTransition::onTransition(event);
}

QString DownloadSM::IDLE = "IDLE";
QString DownloadSM::INIT = "INIT";
QString DownloadSM::DOWNLOADING = "DOWNLOADING";
QString DownloadSM::DOWNLOADING_NOT_CONNECTED = "DOWNLOADING_NOT_CONNECTED";
QString DownloadSM::PAUSED = "PAUSED";
QString DownloadSM::PAUSED_NOT_CONNECTED = "PAUSED_NOT_CONNECTED";
QString DownloadSM::DOWNLOADED = "DOWNLOADED";
QString DownloadSM::HASHING = "HASHING";
QString DownloadSM::POST_PROCESSING = "POST_PROCESSING";
QString DownloadSM::ERROR = "ERROR";
QString DownloadSM::CANCELED = "CANCELED";
QString DownloadSM::FINISHED = "FINISHED";

/**
 * PRIVATE IMPLEMENTATION
 */

class DownloadSMPrivate {
    Q_DECLARE_PUBLIC(DownloadSM)

 public:
    DownloadSMPrivate(SMFileDownload* down, DownloadSM* parent)
        : _down(down),
          q_ptr(parent) {
        Q_Q(DownloadSM);
        _idleState = new State(q, "state", DownloadSM::IDLE);
        _initState = new State(q, "state", DownloadSM::INIT);
        _downloadingState = new State(q, "state", DownloadSM::DOWNLOADING);
        _downloadingNotConnectedState = new State(q, "state",
            DownloadSM::DOWNLOADING_NOT_CONNECTED);
        _pausedState = new State(q, "state", DownloadSM::PAUSED);
        _pausedNotConnectedState = new State(q, "state",
		    DownloadSM::PAUSED_NOT_CONNECTED);
        _downloadedState = new State(q, "state", DownloadSM::DOWNLOADED);
        _hashingState = new State(q, "state", DownloadSM::HASHING);
        _postProcessingState = new State(q, "state", DownloadSM::POST_PROCESSING);
        _errorState = new FinalState(q, "state", DownloadSM::ERROR);
        _canceledState = new FinalState(q, "state", DownloadSM::CANCELED);
        _finishedState = new FinalState(q, "state", DownloadSM::FINISHED);

        // add the idle state transitions
        _transitions.append(new HeaderTransition(_down, _idleState,
	    _initState));
        _idleState->addTransition(_transitions.last());

        _transitions.append(new NetworkErrorTransition(_down, _idleState,
	    _errorState));
        _idleState->addTransition(_transitions.last());

        _transitions.append(new SslErrorTransition(_down, _idleState,
	    _errorState));
        _idleState->addTransition(_transitions.last());

        // add the init state transtions
        _transitions.append(new StartDownloadTransition(_down,
            _initState, _downloadingState));
        _initState->addTransition(_transitions.last());

        _transitions.append(new NetworkErrorTransition(_down,
            _initState, _errorState));
        _initState->addTransition(_transitions.last());

        _transitions.append(new SslErrorTransition(_down,
            _initState, _errorState));
        _initState->addTransition(_transitions.last());

        // add the downloading transitions
        _transitions.append(new PauseRequestTransition(_down,
            SIGNAL(connectionDisabled()), _downloadingState,
            _downloadingNotConnectedState));
        _downloadingState->addTransition(_transitions.last());

        _transitions.append(new PauseRequestTransition(_down,
            SIGNAL(paused()), _downloadingState, _pausedState));
        _downloadingState->addTransition(_transitions.last());

        _transitions.append(new CancelDownloadTransition(_down,
            _downloadingState, _canceledState));
        _downloadingState->addTransition(_transitions.last());

        _transitions.append(new NetworkErrorTransition(_down,
            _downloadingState, _errorState));
        _downloadingState->addTransition(_transitions.last());

        _transitions.append(new SslErrorTransition(_down,
            _downloadingState, _errorState));
        _downloadingState->addTransition(_transitions.last());

        _transitions.append(_downloadingState->addTransition(_down,
            SIGNAL(completed()), _downloadedState));

        // add the downloading not connected transitions
        _transitions.append(new ResumeDownloadTransition(_down,
            SIGNAL(connectionEnabled()),
            _downloadingNotConnectedState,
            _downloadingState));
        _downloadingNotConnectedState->addTransition(_transitions.last());

        _transitions.append(new CancelDownloadTransition(_down,
            _downloadingNotConnectedState, _canceledState));
        _downloadingNotConnectedState->addTransition(_transitions.last());

        _transitions.append(_downloadingNotConnectedState->addTransition(_down,
            SIGNAL(paused()), _pausedNotConnectedState));

        // add the pause transitions
        _transitions.append(new ResumeDownloadTransition(_down,
            SIGNAL(downloadingStarted()), _pausedState, _downloadingState));
        _pausedState->addTransition(_transitions.last());

        _transitions.append(new CancelDownloadTransition(_down,
            _pausedState, _canceledState));
        _pausedState->addTransition(_transitions.last());

        _transitions.append(_pausedState->addTransition(
            _down, SIGNAL(connectionDisabled()), _pausedNotConnectedState));

        // paused not connected transitions
        _transitions.append(new CancelDownloadTransition(
            _down, _pausedNotConnectedState, _canceledState));
        _pausedNotConnectedState->addTransition(_transitions.last());

        _transitions.append(_pausedNotConnectedState->addTransition(
            _down, SIGNAL(downloadingStarted()), _downloadingNotConnectedState));

        _transitions.append(_pausedNotConnectedState->addTransition(_down,
            SIGNAL(connectionEnabled()), _pausedState));

        _transitions.append(_pausedNotConnectedState->addTransition(
            _down, SIGNAL(resumed()), _downloadingNotConnectedState));

        _transitions.append(_pausedNotConnectedState->addTransition(_down,
            SIGNAL(connectionEnabled()), _pausedState));

        // downloaded transitions
        _transitions.append(new CancelDownloadTransition(
            _down, _downloadedState, _canceledState));
        _downloadedState->addTransition(_transitions.last());

        _transitions.append(_downloadedState->addTransition(_down,
            SIGNAL(finished()), _finishedState));

        _transitions.append(_downloadedState->addTransition(_down,
            SIGNAL(hashingStarted()), _hashingState));

        _transitions.append(_downloadedState->addTransition(_down,
            SIGNAL(postProcessingStarted()), _postProcessingState));

        // hashing transitions
        _transitions.append(_hashingState->addTransition(_down,
            SIGNAL(finished()), _finishedState));
        _transitions.append(_hashingState->addTransition(_down,
            SIGNAL(hashingError()), _errorState));
        _transitions.append(_hashingState->addTransition(_down,
            SIGNAL(postProcessingStarted()), _postProcessingState));

        // post processing transitions
        _transitions.append(_postProcessingState->addTransition(
            _down, SIGNAL(finished()), _finishedState));
        _transitions.append(_postProcessingState->addTransition(
            _down, SIGNAL(postProcessingError()), _errorState));

        // add states set init state
        _stateMachine.addState(_idleState);
        _stateMachine.addState(_initState);
        _stateMachine.addState(_downloadingState);
        _stateMachine.addState(_downloadingNotConnectedState);
        _stateMachine.addState(_pausedState);
        _stateMachine.addState(_pausedNotConnectedState);
        _stateMachine.addState(_downloadedState);
        _stateMachine.addState(_hashingState);
        _stateMachine.addState(_postProcessingState);
        _stateMachine.addState(_errorState);
        _stateMachine.addState(_canceledState);
        _stateMachine.addState(_finishedState);

        _stateMachine.setInitialState(_idleState);

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
        DLOG(INFO) << " " << __PRETTY_FUNCTION__ << state;
        Q_Q(DownloadSM);
        _state = state;
        emit q->stateChanged(_state);
    }

    void start() {
        _stateMachine.start();
    }

    ~DownloadSMPrivate() {
        qDeleteAll(_transitions);
        delete _idleState;
        delete _initState;
        delete _downloadingState;
        delete _downloadingNotConnectedState;
        delete _pausedState;
        delete _pausedNotConnectedState;
        delete _downloadedState;
        delete _hashingState;
        delete _postProcessingState;
        delete _errorState;
        delete _canceledState;
        delete _finishedState;
    }

 private:
    QString _state = DownloadSM::IDLE;
    QStateMachine _stateMachine;
    QList<QSignalTransition*> _transitions;
    // states
    State* _idleState;
    State* _initState;
    State* _downloadingState;
    State* _downloadingNotConnectedState;
    State* _pausedState;
    State* _pausedNotConnectedState;
    State* _downloadedState;
    State* _hashingState;
    State* _postProcessingState;
    // final states
    FinalState* _errorState;
    FinalState* _canceledState;
    FinalState* _finishedState;

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

}  // Daemon

}  // DownloadManager

}  // Ubuntu

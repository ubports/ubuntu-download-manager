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
#include "state.h"

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
    DownloadSMTransition::onTransition(event);
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
    TRACE << event;
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
    TRACE << event;
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
 * PRIVATE IMPLEMENATION
 */

class DownloadSMPrivate {
    Q_DECLARE_PUBLIC(DownloadSM)

 public:
    DownloadSMPrivate(SMFileDownload* down, DownloadSM* parent)
        : _down(down),
          q_ptr(parent) {
        Q_Q(DownloadSM);
        _idle = new State(q, "state", DownloadSM::IDLE);
        _init = new State(q, "state", DownloadSM::INIT);
        _downloading = new State(q, "state", DownloadSM::DOWNLOADING);
        _downloadingNotConnected = new State(q, "state",
            DownloadSM::DOWNLOADING_NOT_CONNECTED);
        _paused = new State(q, "state", DownloadSM::PAUSED);
        _pausedNotConnected = new State(q, "state", DownloadSM::PAUSED_NOT_CONNECTED);
        _downloaded = new State(q, "state", DownloadSM::DOWNLOADED);
        _hashing = new State(q, "state", DownloadSM::HASHING);
        _postProcessing = new State(q, "state", DownloadSM::POST_PROCESSING);
        _error = new FinalState(q, "state", DownloadSM::ERROR);
        _canceled = new FinalState(q, "state", DownloadSM::CANCELED);
        _finished = new FinalState(q, "state", DownloadSM::FINISHED);

        // add the idle state transitions
        _transitions.append(new HeaderTransition(_down,
            _idle, _init));
        _idle->addTransition(_transitions.last());

        _transitions.append(new NetworkErrorTransition(
            _down, _idle, _error));
        _idle->addTransition(_transitions.last());

        _transitions.append(new SslErrorTransition(_down,
            _idle, _error));
        _idle->addTransition(_transitions.last());

        // add the init state transtions
        _transitions.append(new StartDownloadTransition(_down,
            _init, _downloading));
        _init->addTransition(_transitions.last());

        _transitions.append(new NetworkErrorTransition(_down,
            _init, _error));
        _init->addTransition(_transitions.last());

        _transitions.append(new SslErrorTransition(_down,
            _init, _error));
        _init->addTransition(_transitions.last());

        // add the downloading transitions
        _transitions.append(new StopRequestTransition(_down,
            SIGNAL(connectionDisabled()), _downloading,
            _downloadingNotConnected));
        _downloading->addTransition(_transitions.last());

        _transitions.append(new StopRequestTransition(_down,
            SIGNAL(paused()), _downloading, _paused));
        _downloading->addTransition(_transitions.last());

        _transitions.append(new CancelDownloadTransition(_down,
            _downloading, _canceled));
        _downloading->addTransition(_transitions.last());

        _transitions.append(new NetworkErrorTransition(_down,
            _downloading, _error));
        _downloading->addTransition(_transitions.last());

        _transitions.append(new SslErrorTransition(_down,
            _downloading, _error));
        _downloading->addTransition(_transitions.last());

        _transitions.append(_downloading->addTransition(_down,
            SIGNAL(completed()), _downloaded));

        // add the downloading not connected transitions
        _transitions.append(new ResumeDownloadTransition(_down,
            SIGNAL(connectionEnabled()),
            _downloadingNotConnected,
            _downloading));
        _downloadingNotConnected->addTransition(_transitions.last());

        _transitions.append(new CancelDownloadTransition(_down,
            _downloadingNotConnected, _canceled));
        _downloadingNotConnected->addTransition(_transitions.last());

        _transitions.append(_downloadingNotConnected->addTransition(_down,
            SIGNAL(paused()), _pausedNotConnected));

        // add the pause transitions
        _transitions.append(new ResumeDownloadTransition(_down,
            SIGNAL(downloadingStarted()), _paused, _downloading));
        _paused->addTransition(_transitions.last());

        _transitions.append(new CancelDownloadTransition(_down,
            _paused, _canceled));
        _paused->addTransition(_transitions.last());

        _transitions.append(_paused->addTransition(
            _down, SIGNAL(connectionDisabled()), _pausedNotConnected));

        // paused not connected transitions
        _transitions.append(new CancelDownloadTransition(
            _down, _pausedNotConnected, _canceled));
        _pausedNotConnected->addTransition(_transitions.last());

        _transitions.append(_pausedNotConnected->addTransition(
            _down, SIGNAL(downloadingStarted()), _downloadingNotConnected));

        _transitions.append(_pausedNotConnected->addTransition(_down,
            SIGNAL(connectionEnabled()), _paused));

        // downloaded transitions
        _transitions.append(new CancelDownloadTransition(
            _down, _downloaded, _canceled));
        _downloaded->addTransition(_transitions.last());

        _transitions.append(_downloaded->addTransition(_down,
            SIGNAL(finished()), _finished));

        _transitions.append(_downloaded->addTransition(_down,
            SIGNAL(hashingStarted()), _hashing));

        _transitions.append(_downloaded->addTransition(_down,
            SIGNAL(postProcessingStarted()), _postProcessing));

        // hashing transitions
        _transitions.append(_hashing->addTransition(_down,
            SIGNAL(finished()), _finished));
        _transitions.append(_hashing->addTransition(_down,
            SIGNAL(hashingError()), _error));
        _transitions.append(_hashing->addTransition(_down,
            SIGNAL(postProcessingStarted()), _postProcessing));

        // post processing transitions
        _transitions.append(_postProcessing->addTransition(
            _down, SIGNAL(finished()), _finished));
        _transitions.append(_postProcessing->addTransition(
            _down, SIGNAL(postProcessingError()), _error));

        // add states set init state
        _stateMachine.addState(_idle);
        _stateMachine.addState(_init);
        _stateMachine.addState(_downloading);
        _stateMachine.addState(_downloadingNotConnected);
        _stateMachine.addState(_paused);
        _stateMachine.addState(_pausedNotConnected);
        _stateMachine.addState(_downloaded);
        _stateMachine.addState(_hashing);
        _stateMachine.addState(_postProcessing);
        _stateMachine.addState(_error);
        _stateMachine.addState(_canceled);
        _stateMachine.addState(_finished);

        _stateMachine.setInitialState(_idle);

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
        TRACE << state;
        Q_Q(DownloadSM);
        _state = state;
        emit q->stateChanged(_state);
    }

    void start() {
        _stateMachine.start();
    }

    ~DownloadSMPrivate() {
        qDeleteAll(_transitions);
        delete _idle;
        delete _init;
        delete _downloading;
        delete _downloadingNotConnected;
        delete _paused;
        delete _pausedNotConnected;
        delete _downloaded;
        delete _hashing;
        delete _postProcessing;
        delete _error;
        delete _canceled;
        delete _finished;
    }

 private:
    QString _state = "IDLE";
    QStateMachine _stateMachine;
    QList<QSignalTransition*> _transitions;
    // states
    State* _idle;
    State* _init;
    State* _downloading;
    State* _downloadingNotConnected;
    State* _paused;
    State* _pausedNotConnected;
    State* _downloaded;
    State* _hashing;
    State* _postProcessing;
    // final states
    QFinalState* _error;
    QFinalState* _canceled;
    QFinalState* _finished;

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

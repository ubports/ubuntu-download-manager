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
#include <QFinalState>
#include <QState>
#include <QStateMachine>
#include <QSslError>
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
    if (e->arguments().count() > 0) {
        QVariant v = e->arguments().at(0);
        QNetworkReply::NetworkError code = v.value<QNetworkReply::NetworkError>();
        down->emitNetworkError(code);
    } else {
        qCritical() << "Signal does not have events!";
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
    SMFileDownload* down = download();
    QStateMachine::SignalEvent* e =
        static_cast<QStateMachine::SignalEvent*>(event);
    if (e->arguments().count() > 0) {
        QVariant v = e->arguments().at(0);
        QList<QSslError> errors = v.value<QList<QSslError> > ();
        down->emitSslError(errors);
    } else {
        qCritical() << "Signal does not have events!";
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
    Q_UNUSED(event);
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
    Q_UNUSED(event);
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
    Q_UNUSED(event);
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
    Q_UNUSED(event);
    SMFileDownload* down = download();
    down->requestDownload();
    down->setState(Download::RESUME);
    DownloadSMTransition::onTransition(event);
}

/**
 * PRIVATE IMPLEMENTATION
 */

class DownloadSMPrivate {
    Q_DECLARE_PUBLIC(DownloadSM)

 public:
    explicit DownloadSMPrivate(DownloadSM* parent)
        : _idleState(),
          _initState(),
          _downloadingState(),
          _downloadingNotConnectedState(),
          _pausedState(),
          _pausedNotConnectedState(),
          _downloadedState(),
          _hashingState(),
          _postProcessingState(),
          _errorState(),
          _canceledState(),
          _finishedState(),
          q_ptr(parent) {
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
    QStateMachine _stateMachine;
    QList<QSignalTransition*> _transitions;
    // states
    QState* _idleState;
    QState* _initState;
    QState* _downloadingState;
    QState* _downloadingNotConnectedState;
    QState* _pausedState;
    QState* _pausedNotConnectedState;
    QState* _downloadedState;
    QState* _hashingState;
    QState* _postProcessingState;
    // final states
    QFinalState* _errorState;
    QFinalState* _canceledState;
    QFinalState* _finishedState;

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

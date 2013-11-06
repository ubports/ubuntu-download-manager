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
        qWarning() << "Signal does not have events!";
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
        qWarning() << "Signal does not have events!";
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
}

/**
 * PRIVATE IMPLEMENATION
 */

class DownloadSMPrivate {
    Q_DECLARE_PUBLIC(DownloadSM)

 public:
    explicit DownloadSMPrivate(DownloadSM* parent)
        : _idle(),
          _init(),
          _downloading(),
          _downloadingNotConnected(),
          _paused(),
          _pausedNotConnected(),
          _downloaded(),
          _hashing(),
          _postProcessing(),
          _error(),
          _canceled(),
          _finished(),
          q_ptr(parent) {
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
            SIGNAL(resumed()), _paused, _downloading));
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
            _down, SIGNAL(resumed()), _downloadingNotConnected));

        _transitions.append(_pausedNotConnected->addTransition(_down,
            SIGNAL(connectionEnabled()), _paused));
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
    QStateMachine _stateMachine;
    QList<QSignalTransition*> _transitions;
    // states
    QState* _idle;
    QState* _init;
    QState* _downloading;
    QState* _downloadingNotConnected;
    QState* _paused;
    QState* _pausedNotConnected;
    QState* _downloaded;
    QState* _hashing;
    QState* _postProcessing;
    // final states
    QFinalState* _error;
    QFinalState* _canceled;
    QFinalState* _finished;

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

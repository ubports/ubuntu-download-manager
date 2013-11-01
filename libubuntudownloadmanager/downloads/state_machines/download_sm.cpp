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
#include <QScopedPointer>
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
          _headerTransition(new HeaderTransition(_down,
                _idle.data(), _init.data())),
          _idleNetworkErrorTransition(new NetworkErrorTransition(
                _down, _idle.data(), _error.data())),
          _idleSslErrorTransition(new SslErrorTransition(_down,
                _idle.data(), _error.data())),
          q_ptr(parent) {

        // add the idle state transitions
        _idle->addTransition(_headerTransition.data());
        _idle->addTransition(_idleNetworkErrorTransition.data());
        _idle->addTransition(_idleSslErrorTransition.data());
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
    // transitions
    QScopedPointer<HeaderTransition> _headerTransition;
    QScopedPointer<NetworkErrorTransition> _idleNetworkErrorTransition;
    QScopedPointer<SslErrorTransition> _idleSslErrorTransition;

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

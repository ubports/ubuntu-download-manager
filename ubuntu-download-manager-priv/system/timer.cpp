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

#include <QTimer>
#include "timer.h"


namespace Ubuntu {

namespace DownloadManager {

namespace System {

Timer::Timer(QObject* parent)
    : QObject(parent) {
    _timer = new QTimer(this);
    _timer->setSingleShot(true);

    connect(_timer, &QTimer::timeout, this, &Timer::timeout);
}

bool
Timer::isActive() {
    return _timer->isActive();
}

void
Timer::start(int msec) {
    _timer->start(msec);
}

void
Timer::stop() {
    _timer->stop();
}

}  // System

}  // DownloadManager

}  // Ubuntu

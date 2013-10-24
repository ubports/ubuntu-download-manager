/*
 * Copyright 2013 2013 Canonical Ltd.
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
#include "./timer.h"

/*
 * PRIVATE IMPLEMENTATION
 */

class TimerPrivate {
    Q_DECLARE_PUBLIC(Timer)

 public:
    explicit TimerPrivate(Timer* parent);
    ~TimerPrivate();

    bool isActive();
    void start(int msec);
    void stop();

 private:
    QTimer* _timer;
    Timer* q_ptr;
};

TimerPrivate::TimerPrivate(Timer* parent)
    : q_ptr(parent) {
    Q_Q(Timer);
    _timer = new QTimer();
    _timer->setSingleShot(true);

    q->connect(_timer, SIGNAL(timeout()),
        q, SIGNAL(timeout()));
}

TimerPrivate::~TimerPrivate() {
    if (_timer)
        delete _timer;
}


bool
TimerPrivate::isActive() {
    return _timer->isActive();
}

void
TimerPrivate::start(int msec) {
    _timer->start(msec);
}

void
TimerPrivate::stop() {
    _timer->stop();
}


/*
 * PUBLIC IMPLEMENTATION
 */

Timer::Timer(QObject *parent)
    : QObject(parent),
      d_ptr(new TimerPrivate(this)) {
}

bool
Timer::isActive() {
    Q_D(Timer);
    return d->isActive();
}

void
Timer::start(int msec) {
    Q_D(Timer);
    d->start(msec);
}

void
Timer::stop() {
    Q_D(Timer);
    d->stop();
}

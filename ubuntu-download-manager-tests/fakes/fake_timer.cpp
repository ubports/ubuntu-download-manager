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

#include "./fake_timer.h"

FakeTimer::FakeTimer(QObject *parent)
    : Timer(parent),
    Fake() {
}

bool
FakeTimer::isActive() {
    if (_recording) {
        QList<QObject*> inParams;

        QList<QObject*> outParams;
        outParams.append(new BoolWrapper(_isActive));

        MethodParams params(inParams, outParams);
        MethodData methodData("isActive", params);
        _called.append(methodData);
    }
    return _isActive;
}

void
FakeTimer::setIsActive(bool active) {
    _isActive = active;
}

void
FakeTimer::start(int msec) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new IntWrapper(msec));

        QList<QObject*> outParams;

        MethodParams params(inParams, outParams);
        MethodData methodData("start", params);
        _called.append(methodData);
    }
}

void
FakeTimer::stop() {
    if (_recording) {
        QList<QObject*> inParams;
        QList<QObject*> outParams;

        MethodParams params(inParams, outParams);
        MethodData methodData("stop", params);
        _called.append(methodData);
    }
}

void
FakeTimer::emitTimeout() {
    emit timeout();
}

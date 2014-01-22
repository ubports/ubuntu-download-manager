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

#include "application.h"

FakeApplication::FakeApplication(QObject *parent)
    : Application(parent),
    Fake() {
}

void
FakeApplication::exit(int returnCode) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new IntWrapper(returnCode, this));

        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("exit", params);
        _called.append(methodData);
    }
}

QStringList
FakeApplication::arguments() {
    if (_recording) {
        QList<QObject*> inParams;
        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("arguments", params);
        _called.append(methodData);
    }
    return _args;
}

void
FakeApplication::setArguments(QStringList args) {
    _args = args;
}

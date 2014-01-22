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

#include "process.h"

OpenModeWrapper::OpenModeWrapper(QProcess::OpenMode mode, QObject* parent)
    : QObject(parent) {
    _value = mode;
}

QProcess::OpenMode
OpenModeWrapper::value() {
    return _value;
}

void
OpenModeWrapper::setValue(QProcess::OpenMode value) {
    _value = value;
}

FakeProcess::FakeProcess(QObject *parent)
    : Process(parent),
    Fake() {
}

void
FakeProcess::start(const QString& program,
                   const QStringList& arguments,
                   QProcess::OpenMode mode) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new StringWrapper(program, this));
        inParams.append(new StringListWrapper(arguments, this));
        inParams.append(new OpenModeWrapper(mode, this));

        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("start", params);
        _called.append(methodData);
    }
}

void
FakeProcess::emitFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    emit finished(exitCode, exitStatus);
}

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

#include <QProcess>
#include "process.h"
#include "logger.h"

namespace Ubuntu {

namespace DownloadManager {

namespace System {


Process::Process(QObject* parent)
    : QObject(parent) {
    _process = new QProcess(this);
    _process->setProcessChannelMode(QProcess::SeparateChannels);

    // connect so that we foward the signals
    connect(_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>
        (&QProcess::finished),
            this, &Process::finished);
    connect(_process, static_cast<void(QProcess::*)(QProcess::ProcessError)>
            (&QProcess::error),
                this, &Process::error);

    // connect so that we can log the stdout and stderr of the process
    connect(_process, &QProcess::readyReadStandardError,
        this, &Process::onReadyReadStandardError);
    connect(_process, &QProcess::readyReadStandardOutput,
        this, &Process::onReadyReadStandardOutput);
}

void
Process::start(const QString& program,
               const QStringList& arguments,
               QProcess::OpenMode mode) {
    LOG(INFO) << __FUNCTION__ << program << arguments << mode;
    _process->start(program, arguments, mode);
}

QString
Process::program() const {
    return _process->program();
}

QStringList
Process::arguments() const {
    return _process->arguments();
}

QByteArray
Process::readAllStandardError() {
    return _process->readAllStandardError();
}

QByteArray
Process::readAllStandardOutput() {
    return _process->readAllStandardOutput();
}

void
Process::onReadyReadStandardError() {
    LOG(ERROR) << _process->readAllStandardError();
}

void
Process::onReadyReadStandardOutput() {
    LOG(INFO) << _process->readAllStandardOutput();
}

}  // System

}  // DownloadManager

}  // Ubuntu

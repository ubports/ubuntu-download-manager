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

#include <QDebug>
#include <QProcess>
#include "process.h"

namespace Ubuntu {

namespace DownloadManager {

namespace System {

/*
 * PRIVATE IMPLEMENTATION
 */

class ProcessPrivate {
    Q_DECLARE_PUBLIC(Process)

 public:
    explicit ProcessPrivate(Process* parent);
    ~ProcessPrivate();

    void start(const QString& program,
               const QStringList& arguments,
               QProcess::OpenMode mode = QProcess::ReadWrite);

    void onReadyReadStandardError();
    void onReadyReadStandardOutput();

 private:
    QProcess* _process;
    Process* q_ptr;
};

ProcessPrivate::ProcessPrivate(Process* parent)
    : q_ptr(parent) {
    Q_Q(Process);
    _process = new QProcess();
    _process->setProcessChannelMode(QProcess::SeparateChannels);

    // connect so that we foward the signals
    q->connect(_process, SIGNAL(finished(int, QProcess::ExitStatus)),
        q, SIGNAL(finished(int, QProcess::ExitStatus)));
    q->connect(_process, SIGNAL(error(QProcess::ProcessError)),
        q, SIGNAL(error(QProcess::ProcessError)));

    // connect so that we can log the stdout and stderr of the process
    q->connect(_process, SIGNAL(readyReadStandardError()),
        q, SLOT(onReadyReadStandardError()));
    q->connect(_process, SIGNAL(readyReadStandardOutput()),
        q, SLOT(onReadyReadStandardOutput()));
}

ProcessPrivate::~ProcessPrivate() {
    delete _process;
}

void
ProcessPrivate::start(const QString& program,
                      const QStringList& arguments,
                      QProcess::OpenMode mode) {
    qDebug() << __FUNCTION__ << program << arguments << mode;
    _process->start(program, arguments, mode);
}

void
ProcessPrivate::onReadyReadStandardError() {
    // use qCritical this is important stuff
    qCritical() << QString(_process->readAllStandardError());
}

void
ProcessPrivate::onReadyReadStandardOutput() {
    // use qDebug
    qDebug() << QString(_process->readAllStandardOutput());
}

/*
 * PUBLIC IMPLEMENTATION
 */

Process::Process(QObject *parent)
    : QObject(parent),
      d_ptr(new ProcessPrivate(this)) {
}

void
Process::start(const QString& program,
                    const QStringList& arguments,
                    QProcess::OpenMode mode) {
    Q_D(Process);
    d->start(program, arguments, mode);
}

}  // System

}  // DownloadManager

}  // Ubuntu

#include "moc_process.cpp"

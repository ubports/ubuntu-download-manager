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

#include <QProcess>
#include "./process.h"

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

    void onError(QProcess::ProcessError error);
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);

 private:
    QProcess* _process;
    Process* q_ptr;
};

ProcessPrivate::ProcessPrivate(Process* parent)
    : q_ptr(parent) {
    Q_Q(Process);
    _process = new QProcess();

    q->connect(_process, SIGNAL(finished(int, QProcess::ExitStatus)),
        q, SLOT(onFinished(int, QProcess::ExitStatus)));
    q->connect(_process, SIGNAL(error(QProcess::ProcessError)),
        q, SLOT(onError(QProcess::ProcessError)));
}

ProcessPrivate::~ProcessPrivate() {
    if (_process != NULL)
        delete _process;
}

void
ProcessPrivate::start(const QString& program,
                      const QStringList& arguments,
                      QProcess::OpenMode mode) {
    _process->start(program, arguments, mode);
}

void
ProcessPrivate::onError(QProcess::ProcessError error) {
    Q_Q(Process);
    emit q->error(error);
}

void
ProcessPrivate::onFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_Q(Process);
    emit q->finished(exitCode, exitStatus);
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

#include "moc_process.cpp"

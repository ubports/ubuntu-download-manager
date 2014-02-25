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

#ifndef FAKE_PROCESS_H
#define FAKE_PROCESS_H

#include <QObject>
#include <ubuntu/system/process.h>
#include "ubuntu/download_manager/tests/fake.h"

using namespace Ubuntu::System;

class OpenModeWrapper: public QObject {
    Q_OBJECT

 public:
    OpenModeWrapper(QProcess::OpenMode mode, QObject* parent = 0);

    QProcess::OpenMode value();
    void setValue(QProcess::OpenMode value);

 private:
    QProcess::OpenMode _value;
};

class FakeProcess : public Process, public Fake {
    Q_OBJECT

 public:
    explicit FakeProcess(QObject *parent = 0);

    void start(const QString& program,
               const QStringList& arguments,
               QProcess::OpenMode mode = QProcess::ReadWrite) override;

    QStringList arguments() const override;
    void setArguments(const QStringList& args);

    QString program() const override;
    void setProgram(const QString& program);

    QByteArray readAllStandardOutput() override;
    void setStandardOutput(const QByteArray& output);

    QByteArray readAllStandardError() override;
    void setStandardError(const QByteArray& err);

    void emitFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void emitError(QProcess::ProcessError error);

 private:
    QString _program;
    QStringList _args;
    QByteArray _stdOut;
    QByteArray _stdErr;
};

#endif  // FAKE_PROCESS_H

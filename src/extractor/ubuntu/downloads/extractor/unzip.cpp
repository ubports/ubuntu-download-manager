/*
 * Copyright 2014 Canonical Ltd.
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

#include <QCoreApplication>
#include <ubuntu/downloads/extractor/unzip.h>

namespace Ubuntu {

namespace DownloadManager {

namespace Extractor {

UnZip::UnZip(const QString& path, const QString& destination,
        QObject* parent)
    : Deflator(path, destination, parent){
}

UnZip::~UnZip() {
    _process->deleteLater();
}

void
UnZip::deflate() {
    // use unzip to extract the file to the destination
     QString program = "unzip";
     QStringList arguments;
     arguments << _path << "-d" << _destination;

     _process = new QProcess(this);

    // connect so that we foward the signals
    connect(_process,
        static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>
            (&QProcess::finished), this, &UnZip::onFinished);
    connect(_process,
        static_cast<void(QProcess::*)(QProcess::ProcessError)>
            (&QProcess::errorOccurred), this, &UnZip::onError);

     _process->start(program, arguments);
}

void
UnZip::onError(QProcess::ProcessError) {
    QCoreApplication::instance()->exit(1);
}

void
UnZip::onFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit || exitCode == 0) {
        QCoreApplication::instance()->exit(0);
    } else {
        QCoreApplication::instance()->exit(exitCode);
    }
}

}

}

}

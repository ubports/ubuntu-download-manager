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

#ifndef UDM_EXTRACTOR_UNZIP_H
#define UDM_EXTRACTOR_UNZIP_H

#include <QProcess>
#include <ubuntu/downloads/extractor/deflator.h>

namespace Ubuntu {

namespace DownloadManager {

namespace Extractor {

class UnZip : public Deflator {
    Q_OBJECT

 public:
    UnZip(const QString& path, const QString& destination,
            QObject* parent=0);
    ~UnZip();

 public slots:  // NOLINT (whitespace/indent)
    void deflate() override;

 private slots:
    void onError(QProcess::ProcessError error);
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);

 private:
    QProcess* _process = nullptr;

};

}

}

}

#endif

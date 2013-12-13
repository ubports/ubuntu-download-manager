/*
 * Copyright 2013 Canonical Ltd.
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
#include <QDir>
#include <QtGlobal>
#include <QStandardPaths>
#include <string.h>
#include "base_testcase.h"

void
noMessageOutput(QtMsgType type,
                const QMessageLogContext& context,
                const QString &message) {
    Q_UNUSED(type);
    Q_UNUSED(context);
    Q_UNUSED(message);
    // do nothing
}

BaseTestCase::BaseTestCase(const QString& testName, QObject *parent)
    : QObject(parent) {
    setObjectName(testName);
}

QString
BaseTestCase::testDirectory() {
    // return the test directory using the standard paths
    QString dataPath = QStandardPaths::writableLocation(
        QStandardPaths::DataLocation);
    QStringList pathComponents;
    pathComponents << dataPath << objectName();
    QString path = pathComponents.join(QDir::separator());

    if (!QDir().exists(path))
        QDir().mkpath(path);

    return path;
}

QString
BaseTestCase::dataDirectory() {
    // get the file name and use it to get the data path that is in the
    // same dir
    QDir dir(".");
    dir.makeAbsolute();

    return dir.path() + "/data";
}

bool
BaseTestCase::removeDir(const QString& dirName) {
    bool result = true;
    QDir dir(dirName);

    QFlags<QDir::Filter> filter =  QDir::NoDotAndDotDot | QDir::System
        | QDir::Hidden  | QDir::AllDirs | QDir::Files;
    if (dir.exists(dirName)) {
        foreach(QFileInfo info, dir.entryInfoList(filter, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            } else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}

void
BaseTestCase::init() {
    qCritical() << "NAseTestCase::init";
    qInstallMessageHandler(noMessageOutput);
    QStandardPaths::enableTestMode(true);
}

void
BaseTestCase::cleanup() {
    // remove the test dir if exists
    removeDir(testDirectory());
}

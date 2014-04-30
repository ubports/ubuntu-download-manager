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

#ifndef DOWNLOADER_LIB_FILENAME_MUTEX_H
#define DOWNLOADER_LIB_FILENAME_MUTEX_H

#include <QMutex>
#include <QObject>
#include <QVariant>
#include <QSet>

namespace Ubuntu {

namespace DownloadManager {

namespace System {

class FileNameMutex : public QObject {
    Q_OBJECT
 public:
    explicit FileNameMutex(QObject* parent = 0);
    virtual QString lockFileName(const QString& expectedName);
    virtual void unlockFileName(const QString& filename);
    virtual bool isLocked(const QString& filename);

    static FileNameMutex* instance();

    // only used for testing so that we can inject a fake
    static void setInstance(FileNameMutex* instance);
    static void deleteInstance();

 protected:
    QSet<QString> _paths;

 private:
    // used for the singleton
    static FileNameMutex* _instance;
    static QMutex _singletonMutex;

    QMutex _mutex;
};

}  // System

}  // DownloadManager

}  // Ubuntu

#endif // FILENAME_MUTEX_H

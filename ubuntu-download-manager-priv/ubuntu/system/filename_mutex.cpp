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

#include <QFile>
#include <QFileInfo>
#include <ubuntu/download_manager/metadata.h>
#include "logger.h"
#include "filename_mutex.h"

namespace Ubuntu {

namespace DownloadManager {

namespace System {

FileNameMutex* FileNameMutex::_instance = nullptr;
QMutex FileNameMutex::_singletonMutex;

FileNameMutex::FileNameMutex(QObject* parent)
    : QObject(parent) {
}

QString
FileNameMutex::lockFileName(const QString& expectedName) {
    auto path = expectedName;
    _mutex.lock();
    QFileInfo fileInfo(expectedName);
    if (!_paths.contains(path) && !QFile::exists(path)) {
        _paths.insert(path);
        _mutex.unlock();
        return path;
    }
    // Split the file into 2 parts - dot+extension, and everything
    // else. For example, "path/file.tar.gz" becomes
    // "path/file"+".tar.gz", while "path/file" (note lack of
    // extension) becomes "path/file"+"".
    auto secondPart = fileInfo.completeSuffix();
    auto firstPart = expectedName;

    if (!secondPart.isEmpty()) {
        secondPart = "." + secondPart;
        firstPart = expectedName.left(expectedName.size()
            - secondPart.size());
    }

    // Try with an ever-increasing number suffix, until we've
    // reached a file that does not yet exist.
    for (int ii = 1; ; ii++) {
        // Construct the new file name by adding the unique
        // number between the first and second part.
        path = QString("%1 (%2)%3").arg(firstPart
            ).arg(ii).arg(secondPart);
        // If no file exists with the new name, return it.
        if (!_paths.contains(path) && !QFile::exists(path)) {
            _paths.insert(path);
            LOG(INFO) << "Locked path '" << path << "'";
            break;
        }
    }  // for

    _mutex.unlock();
    return path;
}

void
FileNameMutex::unlockFileName(const QString& filename) {
    _mutex.lock();
    auto removed = _paths.remove(filename);
    if (!removed) {
        LOG(WARNING) << "Tried to remove filename '" << filename
            << "' when it was not owned by any object.";
    } else {
        LOG(INFO) << "Released path '" << filename << "'";
    }
    _mutex.unlock();
}

bool
FileNameMutex::isLocked(const QString& filename) {
    _mutex.lock();
    auto present = _paths.contains(filename);
    _mutex.unlock();
    return present;
}

FileNameMutex*
FileNameMutex::instance() {
    if(_instance == nullptr) {
        _singletonMutex.lock();
        if(_instance == nullptr)
            _instance = new FileNameMutex();
        _singletonMutex.unlock();
    }
    return _instance;
}

void
FileNameMutex::deleteInstance() {
    if(_instance != nullptr) {
        _singletonMutex.lock();
        if(_instance != nullptr) {
            delete _instance;
            _instance = nullptr;
        }
        _singletonMutex.unlock();
    }
}

void
FileNameMutex::setInstance(FileNameMutex* instance) {
    _instance = instance;
}

}  // System

}  // DownloadManager

}  // Ubuntu

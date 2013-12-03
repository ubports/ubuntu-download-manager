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

#include <QFile>
#include "file_manager.h"

namespace Ubuntu {

namespace DownloadManager {

namespace System {

FileManager* FileManager::_instance = NULL;
QMutex FileManager::_mutex;

bool
FileManager::remove(const QString& path) {
    return QFile::remove(path);
}

bool
FileManager::exists(const QString& path) {
    return QFile::exists(path);
}

FileManager* FileManager::instance() {
    if(_instance == NULL) {
        _mutex.lock();
        if(_instance == NULL)
            _instance = new FileManager();
        _mutex.unlock();
    }
    return _instance;
}

void FileManager::setInstance(FileManager* instance) {
    _instance = instance;
}

void FileManager::deleteInstance() {
    if(_instance != NULL) {
        _mutex.lock();
        if(_instance != NULL) {
            delete _instance;
            _instance = NULL;
        }
        _mutex.unlock();
    }
}

}  // System

}  // DownloadManager

}  // Ubuntu

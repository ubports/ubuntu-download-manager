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

#include <QFile>
#include "file_manager.h"

namespace Ubuntu {

namespace Transfers {

namespace System {

File::File(const QString& name) {
    _file = new QFile(name);
}

File::~File() {
    delete _file;
}

void
File::close() {
    _file->close();
}

QFile::FileError
File::error() const {
    return _file->error();
}

QString
File::fileName() const {
    return _file->fileName();
}

bool
File::flush() {
    return _file->flush();
}

bool
File::open(QIODevice::OpenMode mode) {
    return _file->open(mode);
}

QByteArray
File::readAll() {
    return _file->readAll();
}

bool
File::remove() {
    return _file->remove();
}

bool
File::reset() {
    return _file->reset();
}

qint64
File::size() const {
    return _file->size();
}

qint64
File::write(const QByteArray& byteArray) {
    return _file->write(byteArray);
}


QIODevice*
File::device() {
    return _file;
}

FileManager* FileManager::_instance = nullptr;
QMutex FileManager::_mutex;

File*
FileManager::createFile(const QString& name) {
    return new File(name);
}

bool
FileManager::remove(const QString& path) {
    return QFile::remove(path);
}

bool
FileManager::exists(const QString& path) {
    return QFile::exists(path);
}

bool
FileManager::rename(const QString& oldName, const QString& newName) {
    return QFile::rename(oldName, newName);
}

FileManager* FileManager::instance() {
    if(_instance == nullptr) {
        _mutex.lock();
        if(_instance == nullptr)
            _instance = new FileManager();
        _mutex.unlock();
    }
    return _instance;
}

void FileManager::setInstance(FileManager* instance) {
    _instance = instance;
}

void FileManager::deleteInstance() {
    if(_instance != nullptr) {
        _mutex.lock();
        if(_instance != nullptr) {
            delete _instance;
            _instance = nullptr;
        }
        _mutex.unlock();
    }
}

}  // System

}  // Transfers

}  // Ubuntu

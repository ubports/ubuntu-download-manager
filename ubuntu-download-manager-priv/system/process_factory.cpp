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

#include "process_factory.h"

namespace Ubuntu {

namespace DownloadManager {

namespace System {

ProcessFactory* ProcessFactory::_instance = NULL;
QMutex ProcessFactory::_mutex;

ProcessFactory::ProcessFactory(QObject* parent)
    : QObject(parent) {
}

Process*
ProcessFactory::createProcess() {
    return new Process();
}

ProcessFactory*
ProcessFactory::instance() {
    if(_instance == NULL) {
        _mutex.lock();
        if(_instance == NULL)
            _instance = new ProcessFactory();
        _mutex.unlock();
    }
    return _instance;
}

void
ProcessFactory::deleteInstance() {
    if(_instance != NULL) {
        _mutex.lock();
        if(_instance != NULL) {
            delete _instance;
            _instance = NULL;
        }
        _mutex.unlock();
    }
}

void
ProcessFactory::setInstance(ProcessFactory* instance) {
    _instance = instance;
}

}  // System

}  // DownloadManager

}  // Ubuntu

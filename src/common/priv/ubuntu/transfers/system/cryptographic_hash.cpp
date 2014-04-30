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

#include "cryptographic_hash.h"

namespace Ubuntu {

namespace Transfers {

namespace System {

CryptographicHashFactory* CryptographicHashFactory::_instance = nullptr;
QMutex CryptographicHashFactory::_mutex;

CryptographicHash::CryptographicHash(QCryptographicHash::Algorithm method,
                                     QObject* parent)
    : QObject(parent),
      _hash(method) {
}


bool
CryptographicHash::addData(QIODevice * device) {
    return _hash.addData(device);
}

QByteArray
CryptographicHash::result() const {
    return _hash.result();
}


CryptographicHashFactory::CryptographicHashFactory(QObject* parent)
    : QObject(parent) {
}

CryptographicHash*
CryptographicHashFactory::createCryptographicHash(
                                       QCryptographicHash::Algorithm method,
                                       QObject* parent) {
    return new CryptographicHash(method, parent);
}

CryptographicHashFactory*
CryptographicHashFactory::instance() {
    if(_instance == nullptr) {
        _mutex.lock();
        if(_instance == nullptr)
            _instance = new CryptographicHashFactory();
        _mutex.unlock();
    }
    return _instance;
}

// only used for testing purposes
void
CryptographicHashFactory::setInstance(CryptographicHashFactory* instance) {
    _instance = instance;
}

void
CryptographicHashFactory::deleteInstance() {
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

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

#ifndef DOWNLOADER_LIB_CRYPTOGRAPHIC_HASH_H
#define DOWNLOADER_LIB_CRYPTOGRAPHIC_HASH_H

#include <QByteArray>
#include <QCryptographicHash>
#include <QMutex>
#include <QIODevice>

namespace Ubuntu {

namespace Transfers {

namespace System {

class CryptographicHash : public QObject {

 public:
    CryptographicHash(QCryptographicHash::Algorithm method,
                      QObject* parent = 0);
    virtual bool addData(QIODevice* device);
    virtual QByteArray result() const;

 private:
    QCryptographicHash _hash;
};

class CryptographicHashFactory : public QObject {

 public:
    explicit CryptographicHashFactory(QObject* parent = 0);
    virtual CryptographicHash* createCryptographicHash(
                                       QCryptographicHash::Algorithm method,
                                       QObject* parent = 0);

    static CryptographicHashFactory* instance();

    // only used for testing purposes
    static void setInstance(CryptographicHashFactory* instance);
    static void deleteInstance();

 private:
    static CryptographicHashFactory* _instance;
    static QMutex _mutex;

};

}  // System

}  // Transfers

}  // Ubuntu

#endif

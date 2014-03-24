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

#ifndef FAKE_CRYPTOGRAPHIC_HASH_H
#define FAKE_CRYPTOGRAPHIC_HASH_H

#include <ubuntu/transfers/system/cryptographic_hash.h>
#include <gmock/gmock.h>

namespace Ubuntu {

namespace Transfers {

using namespace System;

namespace Tests {

class MockCryptographicHash : public CryptographicHash {
 public:
    explicit MockCryptographicHash(QObject* parent = 0)
        : CryptographicHash(QCryptographicHash::Md5, parent) {}
    MOCK_METHOD1(addData, bool(QIODevice*));
    MOCK_CONST_METHOD0(result, QByteArray());
};

class MockCryptographicHashFactory : public CryptographicHashFactory {
 public:
    explicit MockCryptographicHashFactory(QObject* parent = 0)
        : CryptographicHashFactory(parent) {}

    MOCK_METHOD2(createCryptographicHash,
            CryptographicHash*(QCryptographicHash::Algorithm, QObject*));
};

}  // Ubuntu

}  // Transfers

}  // Tests
#endif

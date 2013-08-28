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

#include "./hash_algorithm.h"

QCryptographicHash::Algorithm
HashAlgorithm::getHashAlgo(const QString& algorithm) {
    // lowercase the algorithm just in case
    QString algoLower = algorithm.toLower();
    QCryptographicHash::Algorithm algo = QCryptographicHash::Md5;

    if (algoLower == "md5")
        algo = QCryptographicHash::Md5;
    else if (algoLower == "sha1")
        algo = QCryptographicHash::Sha1;
    else if (algoLower == "sha224")
        algo = QCryptographicHash::Sha224;
    else if (algoLower == "sha256")
        algo = QCryptographicHash::Sha256;
    else if (algoLower == "sha384")
        algo = QCryptographicHash::Sha384;
    else if (algoLower == "sha512")
        algo = QCryptographicHash::Sha512;
    else
        algo = QCryptographicHash::Md5;
    return algo;
}

QString
HashAlgorithm::getHashAlgo(QCryptographicHash::Algorithm algorithm) {
    switch (algorithm) {
        case QCryptographicHash::Md5:
            return "md5";
        case QCryptographicHash::Sha1:
            return "sha1";
        case QCryptographicHash::Sha224:
            return "sha224";
        case QCryptographicHash::Sha256:
            return "shq256";
        case QCryptographicHash::Sha384:
            return "sha384";
        case QCryptographicHash::Sha512:
            return "sha512";
        default:
            return "";
    }
}

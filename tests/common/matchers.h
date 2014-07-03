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

#ifndef MATCHERS_H
#define MATCHERS_H

#include <QMap>
#include <QNetworkRequest>
#include <QPair>
#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <gmock/gmock.h>

namespace {
    template<typename T>
    bool compareMaps(T firstMap, T secondMap) {
        foreach(const QString& key, firstMap.keys()) {
            if (secondMap.contains(key)) {
                if (firstMap[key] != secondMap[key])
                    return false;
            } else {
                return false;
            }
        }
        return true;
    }
}


MATCHER_P(QVariantMapEq, value, "Returns if the variant maps are equal.") {
    auto argMap = static_cast<QVariantMap>(arg);
    auto valueMap = static_cast<QVariantMap>(value);
    return compareMaps<QVariantMap>(argMap, valueMap);
}

MATCHER_P(QStringMapEq, value, "Returns if the string maps are equal.") {
    auto argMap = static_cast<QMap<QString, QString> >(arg);
    auto valueMap = static_cast<QMap<QString, QString> >(value);
    return compareMaps<QMap<QString, QString> >(argMap, valueMap);
}

MATCHER_P(QVariantMapContains, value, "Returns if the map has the given vkey/value pair") {
    auto argMap = static_cast<QVariantMap>(arg);
    auto valuePair = static_cast<QPair<QString, QVariant> >(value);
    if (!argMap.contains(valuePair.first)) {
        return false;
    }
    return argMap[valuePair.first] == valuePair.second;
}

MATCHER_P(RequestHeadersEq, value, "Request has the header.") {
    auto request = static_cast<QNetworkRequest>(arg);
    auto headers = static_cast<QMap<QString, QString> >(value);

    foreach(auto headerKey, headers.keys()) {
        auto key = headerKey.toUtf8();
        // assert that they are the same
        if (request.hasRawHeader(key)) {
            if (request.rawHeader(key) != headers[headerKey]) {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}

MATCHER_P(RequestDoesNotHaveHeader, value, "Returns if the request does not have a given header.") {
    auto request = static_cast<QNetworkRequest>(arg);
    auto header = static_cast<QString>(value);
    return !request.hasRawHeader(header.toUtf8());
}

MATCHER_P(RequestHasHeader, value, "Returns if the request does not have a given header.") {
    auto request = static_cast<QNetworkRequest>(arg);
    auto headerPair = static_cast<QPair<QString, QString> >(value);
    auto header = headerPair.first.toUtf8();

    if (request.hasRawHeader(header)) {
        // values must be the same
        return request.rawHeader(header) == headerPair.second.toUtf8();
    } else {
        return false;
    }
}

MATCHER_P(StringListEq, value, "Returns if the string lists are eq.") {
    auto list = static_cast<QStringList>(arg);
    auto expectedList = static_cast<QStringList>(value);

    // compare the lists
    if (list.count() != expectedList.count()) {
        return false;
    } else {
        foreach(auto str, expectedList) {
            if (!list.contains(str)) {
                return false;
            }
        }
        return true;
    }
}

MATCHER_P(QStringEndsWith, value, "Returns if the string has the given postfix.") {
    auto str = static_cast<QString>(arg);
    auto post = static_cast<QString>(value);
    return str.endsWith(post);
}

#endif


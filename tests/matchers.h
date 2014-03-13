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
#include <QString>
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


MATCHER_P(QVariantMapEq, value, "") {
    auto argMap = static_cast<QVariantMap>(arg);
    auto valueMap = static_cast<QVariantMap>(value);
    return compareMaps<QVariantMap>(argMap, valueMap);
}

MATCHER_P(QStringMapEq, value, "") {
    auto argMap = static_cast<QMap<QString, QString> >(arg);
    auto valueMap = static_cast<QMap<QString, QString> >(value);
    return compareMaps<QMap<QString, QString> >(argMap, valueMap);
}

#endif


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

#include <QStringList>

#include <set>

#include "header_parser.h"

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

QString
HeaderParser::fileNameFromContentDisposition(QString value) {
    auto keyValuePairs = value.split(';');

    foreach(const QString& valuePair, keyValuePairs) {
        int valueStartPos = valuePair.indexOf('=');

        if (valueStartPos < 0)
            continue;

        auto pair = valuePair.split('=');
        if (pair.size() != 2 || pair[0].isEmpty() || pair[0].simplified() != "filename")
            continue;

        auto value = pair[1].replace("\"", "") // remove ""
            .replace("'", "") // remove '
            .simplified();  // remove white spaces
        return value;
    }

    return QString();
}

bool
HeaderParser::isDeflatableContentType(QString value) {
    // build set with the known deflatable content types
    static std::set<std::string> types {
        "application/x-7z-compressed",
        "application/x-ace-compressed",
        "application/x-bzip",
        "application/x-bzip2",
        "application/x-gtar",
        "application/zip"
    };
    return types.find(value.toStdString()) != types.end();
}

}  // Daemon

}  // DownloadManager

}  // Ubuntu

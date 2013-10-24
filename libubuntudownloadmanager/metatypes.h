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

#ifndef DOWNLOADER_LIB_APP_METATYPES_H
#define DOWNLOADER_LIB_APP_METATYPES_H

#include <QMap>
#include <QList>
#include "downloads/group_download_struct.h"
#include "downloads/download_struct.h"

typedef QMap<QString, QString> StringMap;
typedef QList<GroupDownloadStruct> StructList;

Q_DECLARE_METATYPE(DownloadStruct)
Q_DECLARE_METATYPE(StringMap)
Q_DECLARE_METATYPE(StructList)

#endif  // DOWNLOADER_LIB_APP_METATYPES_H

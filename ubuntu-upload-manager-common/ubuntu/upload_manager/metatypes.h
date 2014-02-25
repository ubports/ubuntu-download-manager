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

#ifndef UPLOADER_LIB_APP_METATYPES_H
#define UPLOADER_LIB_APP_METATYPES_H

#include <QMap>
#include <QList>
#include "upload_struct.h"

using namespace Ubuntu::UploadManager;

typedef QMap<QString, QString> StringMap;

Q_DECLARE_METATYPE(UploadStruct)
Q_DECLARE_METATYPE(StringMap)

#endif  // UPLOADER_LIB_APP_METATYPES_H

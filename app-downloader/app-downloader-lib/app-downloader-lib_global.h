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


#ifndef APPDOWNLOADERLIB_GLOBAL_H
#define APPDOWNLOADERLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(APPDOWNLOADERLIB_LIBRARY)
#  define APPDOWNLOADERLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define APPDOWNLOADERLIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // APPDOWNLOADERLIB_GLOBAL_H

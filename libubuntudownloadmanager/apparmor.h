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

#ifndef DOWNLOADER_LIB_APP_ARMOR_H
#define DOWNLOADER_LIB_APP_ARMOR_H

#include <QString>
#include <QObject>

class AppArmorPrivate;
class AppArmor : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(AppArmor)

 public:
    explicit AppArmor(QObject *parent = 0);

    virtual QString getSecurePath(QString connName);

 private:
    // use pimpl so that we can mantains ABI compatibility
    AppArmorPrivate* d_ptr;
};

#endif  // DOWNLOADER_LIB_APP_ARMOR_H

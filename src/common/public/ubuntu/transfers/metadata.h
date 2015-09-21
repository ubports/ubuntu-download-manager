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

#pragma once

#include <QMap>
#include <QVariant>

namespace Ubuntu {

namespace Transfers {

class Metadata : public QVariantMap {

 public:
    Metadata();
    Metadata(const QVariantMap map);

    static const QString COMMAND_KEY;
    static const QString COMMAND_FILE_KEY;
    static const QString LOCAL_PATH_KEY;
    static const QString OBJECT_PATH_KEY;
    static const QString TITLE_KEY;
    static const QString SHOW_IN_INDICATOR_KEY;
    static const QString CLICK_PACKAGE_KEY;
    static const QString DEFLATE_KEY;
    static const QString EXTRACT_KEY;
    static const QString APP_ID;

    // accessors to simplify the use of the metadata
    QString command() const;
    void setCommand(const QString& command);
    bool hasCommand() const;

    QString localPath() const;
    void setLocalPath(const QString& localPath);
    bool hasLocalPath() const;

    QString objectPath() const;
    void setObjectPath(const QString& path);
    bool hasObjectPath() const;

    QString title() const;
    void setTitle(const QString& title);
    bool hasTitle() const;

    bool showInIndicator() const;
    void setShowInIndicator(bool shown);
    bool hasShowInIndicator() const;

    QString clickPackage() const;
    void setClickPackage(const QString& click);
    bool hasClickPackage() const;

    bool deflate() const;
    void setDeflate(bool deflate);
    bool hasDeflate() const;

    bool extract() const;
    void setExtract(bool extract);
    bool hasExtract() const;

    QString destinationApp() const;
    void setOwner(const QString &id);
    bool hasOwner() const;
};

}  // DownloadManager

}  // Ubuntu

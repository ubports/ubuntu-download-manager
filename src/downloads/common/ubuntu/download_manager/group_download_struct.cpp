/*
 * copyright 2013 2013 canonical ltd.
 *
 * this library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the gnu lesser general public
 * license as published by the free software foundation.
 *
 * this program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * merchantability or fitness for a particular purpose.  see the gnu
 * general public license for more details.
 *
 * you should have received a copy of the gnu lesser general public
 * license along with this library; if not, write to the
 * free software foundation, inc., 51 franklin street, fifth floor,
 * boston, ma 02110-1301, usa.
 */

#include <QDBusMetaType>
#include <QDBusArgument>
#include <ubuntu/transfers/system/hash_algorithm.h>
#include "group_download_struct.h"

namespace Ubuntu {

namespace DownloadManager {

GroupDownloadStruct::GroupDownloadStruct()
    : _url(""),
      _hash("") {
}

GroupDownloadStruct::GroupDownloadStruct(const QString& url,
                                         const QString& localFile,
                                         const QString& hash)
    : _url(url),
      _localFile(localFile),
      _hash(hash) {
}

GroupDownloadStruct::GroupDownloadStruct(const GroupDownloadStruct& other)
    : _url(other._url),
      _localFile(other._localFile),
      _hash(other._hash) {
}

GroupDownloadStruct& GroupDownloadStruct::operator=(
                                  const GroupDownloadStruct& other) {
    _url = other._url;
    _localFile = other._localFile;
    _hash = other._hash;

    return *this;
}

GroupDownloadStruct::~GroupDownloadStruct() {
}


QDBusArgument &operator<<(QDBusArgument& argument,
                          const GroupDownloadStruct& group) {
    argument.beginStructure();
    argument << group._url;
    argument << group._localFile;
    argument << group._hash;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument& argument,
                                GroupDownloadStruct& group) {
    argument.beginStructure();
    argument >> group._url;
    argument >> group._localFile;
    argument >> group._hash;
    argument.endStructure();

    return argument;
}

void GroupDownloadStruct::registerMetaType() {
    qRegisterMetaType<GroupDownloadStruct>("GroupDownloadStruct");
    qDBusRegisterMetaType<GroupDownloadStruct>();
}

QString
GroupDownloadStruct::getUrl() const {
    return _url;
}

QString
GroupDownloadStruct::getHash() const {
    return _hash;
}

QString
GroupDownloadStruct::getLocalFile() const {
    return _localFile;
}

}  // DownloadManager

}  // Ubuntu

/*
 * Copyright 2013-2014 Canonical Ltd.
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

#ifndef DOWNLOADER_LIB_FILE_MANAGER_H
#define DOWNLOADER_LIB_FILE_MANAGER_H

#include <QIODevice>
#include <QFile>
#include <QMutex>
#include <QObject>

namespace Ubuntu {

namespace Transfers {

namespace System {

class File : public QObject {
    Q_OBJECT

    // only allow the use of the file manager to create files
    friend class FileManager;
 public:
    virtual ~File();

    // wrappers around the used QFile methods
    virtual void close();
    virtual QFile::FileError error() const;  // virtual for testing purposes
    virtual QString fileName() const;
    virtual bool flush();  // virtual for testing purposes
    virtual bool open(QIODevice::OpenMode mode);
    virtual QByteArray readAll();
    virtual bool remove();
    virtual bool reset();
    virtual qint64 size() const;
    virtual qint64 write(const QByteArray& byteArray);
    virtual QIODevice* device();

 protected:
    explicit File(const QString& name);

 private:
    QFile* _file = nullptr;

};

class FileManager : public QObject {
    Q_OBJECT

 public:
    virtual File* createFile(const QString& name);
    virtual bool remove(const QString& path);
    virtual bool exists(const QString& path);
    virtual bool rename(const QString& oldName, const QString& newName);

    static FileManager* instance();

    // only used for testing so that we can inject a fake
    static void setInstance(FileManager* instance);
    static void deleteInstance();

 protected:
    explicit FileManager(QObject *parent = 0) : QObject(parent) {}

 private:
    // used for the singleton
    static FileManager* _instance;
    static QMutex _mutex;

};

}  // System

}  // Transfers

}  // Ubuntu

#endif  // DOWNLOADER_LIB_FILE_MANAGER_H

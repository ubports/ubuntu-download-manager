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

#ifndef UPLOADER_LIB_DOWNLOADER_H
#define UPLOADER_LIB_DOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QtDBus/QDBusObjectPath>
#include <QSslCertificate>
#include <ubuntu/upload_manager/metatypes.h>
#include <ubuntu/transfers/system/application.h>
#include <ubuntu/transfers/system/dbus_connection.h>
#include <ubuntu/transfers/queue.h>
#include <ubuntu/transfers/base_manager.h>
#include "factory.h"

namespace Ubuntu {

using namespace Transfers;

namespace UploadManager {

namespace Daemon {

class UploadManager : public BaseManager {
    Q_OBJECT

 public:
    UploadManager(Application* app,
                  DBusConnection* connection,
                  bool stoppable = false,
                  QObject *parent = 0);
    UploadManager(Application* app,
                  DBusConnection* connection,
                  Factory* downloadFactory,
                  Queue* queue,
                  bool stoppable = false,
                  QObject *parent = 0);

    virtual ~UploadManager();

    virtual QList<QSslCertificate> acceptedCertificates();
    virtual void setAcceptedCertificates(const QList<QSslCertificate>& certs);
    static const QString SERVICE_PATH;

 public slots:
    void allowMobileUpload(bool allowed);
    QDBusObjectPath createMmsUpload(const QString& url,
                                    const QString& file,
                                    const QString& hostname,
                                    int port,
                                    const QString& username,
                                    const QString& password);
    QDBusObjectPath createUpload(const QString& url,
                                 const QString& filePath,
                                 const QVariantMap& metadata,
                                 StringMap headers);
    QDBusObjectPath createUpload(UploadStruct upload);
    qulonglong defaultThrottle();
    QList<QDBusObjectPath> getAllUploads();
    QList<QDBusObjectPath> getAllUploadsWithMetadata(
                                        const QString& name,
                                        const QString& value);
    bool isMobileUploadAllowed();
    void setDefaultThrottle(qulonglong speed);

 signals:
    void uploadCreated(const QDBusObjectPath& path);
    void sizeChanged(int size);

 private:
    typedef std::function<FileUpload*(QString)> UploadCreationFunc;

    QDBusObjectPath createUpload(UploadCreationFunc createUploadFunc);
    void onUploadsChanged(QString);
    QDBusObjectPath registerUpload(FileUpload* upload);

    void init();


 private:
    Application* _app = nullptr;
    qulonglong _throttle;
    Factory* _factory = nullptr;
    Queue* _queue = nullptr;
    DBusConnection* _conn = nullptr;
    bool _stoppable = false;
    bool _allowMobileData = true;
};

}  // Daemon

}  // UploadManager

}  // Ubuntu

#endif

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

#ifndef APP_DOWNLOADER_LIB_REQUEST_FACTORY_H
#define APP_DOWNLOADER_LIB_REQUEST_FACTORY_H

#include <QNetworkRequest>
#include <QObject>
#include "network_reply.h"
#include "app-downloader-lib_global.h"

class RequestFactoryPrivate;
class APPDOWNLOADERLIBSHARED_EXPORT RequestFactory : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RequestFactory)

public:
    explicit RequestFactory(QObject *parent = 0);
    
    virtual NetworkReply* get(const QNetworkRequest& request);

private:
    // use pimpl so that we can mantains ABI compatibility
    RequestFactoryPrivate* d_ptr;
};

#endif // REQUEST_FACTORY_H

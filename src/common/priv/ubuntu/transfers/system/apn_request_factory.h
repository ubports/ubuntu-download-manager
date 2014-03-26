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

#ifndef DOWNLOADER_LIB_APN_REQUEST_FACTORY_H
#define DOWNLOADER_LIB_APN_REQUEST_FACTORY_H

#include <QNetworkProxy>
#include <QObject>
#include "request_factory.h"

namespace Ubuntu {

namespace Transfers {

namespace System {

class ApnRequestFactory : public RequestFactory {
    Q_OBJECT

 public:
    ApnRequestFactory(const QNetworkProxy& proxy,
                      bool stoppable = false,
                      QObject* parent=0);
};

}  // System

}  // Transfers

}  // Ubuntu

#endif // APN_REQUEST_FACTORY_H

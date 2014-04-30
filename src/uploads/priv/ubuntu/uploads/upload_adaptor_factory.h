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

#ifndef UPLOAD_ADAPTOR_FACTORY_H
#define UPLOAD_ADAPTOR_FACTORY_H

#include <QObject>
#include <ubuntu/transfers/adaptor_factory.h>

namespace Ubuntu {

using namespace Transfers;

namespace UploadManager {

namespace Daemon {

class UploadAdaptorFactory : public AdaptorFactory {
    Q_OBJECT

 public:
    explicit UploadAdaptorFactory(QObject *parent = 0);

    virtual QObject* createAdaptor(BaseManager* man);
};

}  // Daemon

}  // UploadManager

}  // Ubuntu
#endif // DOWNLOAD_ADAPTOR_FACTORY_H


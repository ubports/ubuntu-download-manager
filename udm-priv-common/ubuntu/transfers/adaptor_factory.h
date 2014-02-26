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

#ifndef UBUNTU_GENERAL_ADAPTOR_FACTORY_H
#define UBUNTU_GENERAL_ADAPTOR_FACTORY_H

#include <QObject>
#include "base_manager.h"

namespace Ubuntu {

namespace Transfers {

class AdaptorFactory : public QObject {
 public:
    explicit AdaptorFactory(QObject *parent = 0)
        : QObject(parent) {}

    virtual QObject* createAdaptor(BaseManager* man) = 0;
};

}  // General

}  // Ubuntu

#endif // UBUNTU_GENERAL_ADAPTOR_FACTORY_H

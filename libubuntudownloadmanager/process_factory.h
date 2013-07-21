/*
 * Copyright 2013 2013 Canonical Ltd.
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

#ifndef DOWNLOADER_LIB_PROCESS_FACTORY_H
#define DOWNLOADER_LIB_PROCESS_FACTORY_H

#include <QObject>
#include "process.h"

class ProcessFactoryPrivate;
class ProcessFactory : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ProcessFactory)

public:
    explicit ProcessFactory(QObject *parent = 0);

    virtual Process* createProcess();

private:
    // use pimpl so that we can mantains ABI compatibility
    ProcessFactoryPrivate* d_ptr;
};

#endif

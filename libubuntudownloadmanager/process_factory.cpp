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

#include "process_factory.h"

/*
 * PRIVATE IMPLEMENTATION
 */

class ProcessFactoryPrivate
{
    Q_DECLARE_PUBLIC(ProcessFactory)
public:
    explicit ProcessFactoryPrivate(ProcessFactory* parent);

    Process* createProcess();

private:
    ProcessFactory* q_ptr;

};


ProcessFactoryPrivate::ProcessFactoryPrivate(ProcessFactory* parent):
    q_ptr(parent)
{
}

Process* ProcessFactoryPrivate::createProcess()
{
    return new Process();
}

/*
 * PUBLIC IMPLEMENTATION
 */

ProcessFactory::ProcessFactory(QObject *parent) :
    QObject(parent),
    d_ptr(new ProcessFactoryPrivate(this))
{
}

Process* ProcessFactory::createProcess()
{
    Q_D(ProcessFactory);
    return d->createProcess();
}

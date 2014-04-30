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

#ifndef FILENAME_MUTEXT_H
#define FILENAME_MUTEXT_H

#include <QObject>
#include <ubuntu/transfers/system/filename_mutex.h>
#include "ubuntu/download_manager/tests/fake.h"

using namespace Ubuntu::System;

class FakeFileNameMutex : public FileNameMutex, public Fake {
    Q_OBJECT

 public:
    explicit FakeFileNameMutex(QObject *parent = 0);

    /// override to keep track of the calls
    QString lockFileName(const QString& expectedName) override;
    void unlockFileName(const QString& filename) override;

    // just for testing purposes
    void clearMutex();

};

#endif // FILENAME_MUTEXT_H

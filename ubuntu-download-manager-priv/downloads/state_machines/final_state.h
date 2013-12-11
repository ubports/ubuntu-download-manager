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

#ifndef DOWNLOADER_LIB_FINAL_STATE_H
#define DOWNLOADER_LIB_FINAL_STATE_H

#include <QEvent>
#include <QFinalState>
#include <QObject>
#include <QVariant>

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

namespace StateMachines {

struct PropertyAssignment {

    PropertyAssignment()
        : object(0), explicitlySet(true) {}

    PropertyAssignment(QObject *o, const QByteArray &n,
                        const QVariant &v, bool es = true)
        : object(o), propertyName(n), value(v), explicitlySet(es)
        {}

    QObject* object;
    QByteArray propertyName;
    QVariant value;
    bool explicitlySet;
};

class FinalState : public QFinalState {
    Q_OBJECT
 public:
    explicit FinalState(QState* parent = 0);
    FinalState(QObject* obj,
               const char* property,
               QVariant value,
               QState* parent = 0);

    void assignProperty(QObject *object,
                        const char* name,
                        const QVariant& value);

    void onEntry(QEvent* event) override;

 private:
    QList<PropertyAssignment> _propertyAssignments;
    
};

}  // StateMachines

}  // Daemon

}  // DownloadManager

}  // Ubuntu

#endif // FINAL_STATE_H

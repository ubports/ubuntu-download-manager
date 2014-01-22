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

#include <QDebug>
#include "final_state.h"

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

namespace StateMachines {

FinalState::FinalState(QState* parent)
    : QFinalState(parent) {
}

FinalState::FinalState(QObject* obj,
                       const char* property,
                       QVariant value,
                       QState* parent)
    : QFinalState(parent) {
    assignProperty(obj, property, value);
}

void
FinalState::assignProperty(QObject *object,
                           const char* name,
                           const QVariant& value) {
    if (!object) {
        qWarning() << "FinalState::assignProperty: cannot assign property"
            << name << "of null object";
        return;
    }
    for (int i = 0; i < _propertyAssignments.size(); ++i) {
        PropertyAssignment& assn = _propertyAssignments[i];
        if ((assn.object == object) && (assn.propertyName == name)) {
            assn.value = value;
            return;
        }
    }
    _propertyAssignments.append(PropertyAssignment(object, name, value));
}

void
FinalState::onEntry(QEvent* event) {
    // set all the diff properties
    foreach(PropertyAssignment assn, _propertyAssignments) {
        assn.object->setProperty(assn.propertyName, assn.value);
    }

    QFinalState::onEntry(event);
}

}  // StateMachines

}  // Daemon

}  // DownloadManager

}  // Ubuntu

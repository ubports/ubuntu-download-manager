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

#include "./fake_dbus_connection.h"

/*
 * REGISTER OPTIONS WRAPPER
 */

RegisterOptionsWrapper::RegisterOptionsWrapper(
                QDBusConnection::RegisterOptions options,
                QObject* parent)
    : QObject(parent),
      _value(options) {
}

QDBusConnection::RegisterOptions
RegisterOptionsWrapper::value() {
    return _value;
}

void
RegisterOptionsWrapper::setValue(QDBusConnection::RegisterOptions value) {
    _value = value;
}

/*
 * UNREGISTER OPTIONS WRAPPER
 */

UnregisterOptionsWrapper::UnregisterOptionsWrapper(
                QDBusConnection::UnregisterMode options,
                QObject* parent)
    : QObject(parent),
      _value(options) {
}

QDBusConnection::UnregisterMode
UnregisterOptionsWrapper::value() {
    return _value;
}

void
UnregisterOptionsWrapper::setValue(QDBusConnection::UnregisterMode value) {
    _value = value;
}

/*
 * FAKE DBUS CONNECTION
 */

FakeDBusConnection::FakeDBusConnection(QObject *parent)
    : DBusConnection(parent),
    Fake() {
    _registerServiceResult = false;
    _registerObjectResult = false;
}

bool
FakeDBusConnection::registerService(const QString& serviceName) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new StringWrapper(serviceName, this));

        QList<QObject*> outParams;
        outParams.append(new BoolWrapper(_registerServiceResult, this));
        MethodParams params(inParams, outParams);

        MethodData methodData("registerService", params);
        _called.append(methodData);
    }
    return _registerServiceResult;
}

bool
FakeDBusConnection::registerObject(const QString& path,
                                   QObject* object,
                                   QDBusConnection::RegisterOptions options) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new StringWrapper(path, this));
        inParams.append(object);
        inParams.append(new RegisterOptionsWrapper(options, this));

        QList<QObject*> outParams;
        outParams.append(new BoolWrapper(_registerObjectResult, this));
        MethodParams params(inParams, outParams);

        MethodData methodData("registerObject", params);
        _called.append(methodData);
    }
    return _registerObjectResult;
}

void
FakeDBusConnection::unregisterObject(const QString& path,
                                     QDBusConnection::UnregisterMode mode) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new StringWrapper(path, this));
        inParams.append(new UnregisterOptionsWrapper(mode, this));

        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);

        MethodData methodData("unregisterObject", params);
        _called.append(methodData);
    }
}

bool
FakeDBusConnection::registerServiceResult() {
    return _registerServiceResult;
}

void
FakeDBusConnection::setRegisterServiceResult(bool result) {
    _registerServiceResult = result;
}

bool
FakeDBusConnection::registerObjectResult() {
    return _registerObjectResult;
}

void
FakeDBusConnection::setRegisterObjectResult(bool result) {
    _registerObjectResult = result;
}

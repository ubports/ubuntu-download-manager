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

#ifndef FAKE_DBUS_CONNECTION_H
#define FAKE_DBUS_CONNECTION_H
#include <dbus_connection.h>
#include "./fake.h"

class RegisterOptionsWrapper : public QObject {
    Q_OBJECT

 public:
    explicit RegisterOptionsWrapper(QDBusConnection::RegisterOptions options,
                                    QObject* parent = 0);

    QDBusConnection::RegisterOptions value();
    void setValue(QDBusConnection::RegisterOptions value);

 private:
    QDBusConnection::RegisterOptions _value;
};

class UnregisterOptionsWrapper : public QObject {
    Q_OBJECT
 public:
    explicit UnregisterOptionsWrapper(QDBusConnection::UnregisterMode options,
                                      QObject* parent = 0);

    QDBusConnection::UnregisterMode value();
    void setValue(QDBusConnection::UnregisterMode value);

 private:
    QDBusConnection::UnregisterMode _value;
};

class FakeDBusConnection : public DBusConnection, public Fake {
    Q_OBJECT
 public:
    explicit FakeDBusConnection(QObject *parent = 0);

    // faked methods

    bool registerService(const QString& serviceName) override;
    bool registerObject(const QString& path,
                        QObject* object,
                        QDBusConnection::RegisterOptions options = QDBusConnection::ExportAdaptors) override;  // NOLINT(whitespace/line_length)
    void unregisterObject(const QString& path,
                          QDBusConnection::UnregisterMode mode = QDBusConnection::UnregisterNode) override;  // NOLINT(whitespace/line_length)

    // getters and setters of expected results
    bool registerServiceResult();
    void setRegisterServiceResult(bool result);

    bool registerObjectResult();
    void setRegisterObjectResult(bool result);

 private:
    bool _registerServiceResult;
    bool _registerObjectResult;
};

#endif  // FAKE_DBUS_CONNECTION_H

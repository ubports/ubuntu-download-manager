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

#ifndef BASE_TESTCASE_H
#define BASE_TESTCASE_H

#include <QEventLoop>
#include <QSignalSpy>
#include <QTest>
#include <QTimer>
#include <QObject>

namespace Ubuntu {

namespace Transfers {

namespace Tests {

// util methods used to minimize the issues with signals in diff platforms
static inline bool waitForSignal(QObject* obj, const char* signal, int timeout = -1) {
    QEventLoop loop;
    QObject::connect(obj, signal, &loop, SLOT(quit()));
    QTimer timer;
    QSignalSpy timeoutSpy(&timer, SIGNAL(timeout()));
    if (timeout > 0) {
        QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
        timer.setSingleShot(true);
        timer.start(timeout);
    }
    loop.exec();
    return timeoutSpy.isEmpty();
}

class SignalBarrier : public QSignalSpy {
 public:
    SignalBarrier(const QObject* obj, const char* aSignal)
        : QSignalSpy(obj, aSignal) {}

    bool ensureSignalEmitted() {
        bool result = count() > 0;
        if (!result) {
            result = wait(-1);
        }
        return result;
    }
};

class BaseTestCase : public QObject {
    Q_OBJECT

 public:
    BaseTestCase(const QString& testName, QObject *parent = 0);

    QString testDirectory();
    QString dataDirectory();

 protected slots:  // NOLINT(whitespace/indent)

    virtual void init();
    virtual void cleanup();

 private:
    bool removeDir(const QString& dirName);
};

}  // Ubuntu

}  // Transfers

}  // Tests

#endif // BASE_TESTCASE_H

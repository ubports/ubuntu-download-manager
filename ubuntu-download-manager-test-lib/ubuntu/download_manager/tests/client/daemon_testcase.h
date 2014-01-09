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

#ifndef DAEMON_TESTCASE_H
#define DAEMON_TESTCASE_H

#include <QDebug>
#include <QThread>
#include <QObject>
#include "ubuntu/download_manager/tests/base_testcase.h"
#include "testing_daemon.h"

using namespace Ubuntu::DownloadManager;

class DaemonThread : public QThread {
    Q_OBJECT

 public:
    DaemonThread(QString path, QObject* parent=0)
        : QThread(parent),
          _path(path) {
    }

    virtual ~DaemonThread() {
        _daemon->stop();
        delete _daemon;
    }

    void returnDBusErrors(bool errors) {
        _daemon->returnDBusErrors(errors);
    }

 protected:
    void run() override {
        _daemon = new TestingDaemon();
        _daemon->enableTimeout(false);
        qDebug() << "Star daemon" << _path;
        _daemon->start(_path);
        QThread::run();
    }

 private:
    QString _path;
    bool _started = false;
    TestingDaemon* _daemon = NULL;
};

class DaemonTestCase : public BaseTestCase {
    Q_OBJECT

 public:
    DaemonTestCase(const QString& testName, QObject *parent = 0);

    QString daemonPath();

 protected slots:  // NOLINT(whitespace/indent)

    void init() override;
    virtual void initTestCase();
    void cleanup() override;
    virtual void cleanupTestCase();
    void returnDBusErrors(bool errors);

 signals:
    void daemonStarted();
    void daemonStopped();

 private:
    QString _daemonPath;
    DaemonThread* _daemonThread;
};

#endif // DAEMON_TESTCASE_H

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

#include <QDebug>
#include <QCoreApplication>
#include "application.h"

namespace Ubuntu {

namespace DownloadManager {

namespace System {

/*
 * PRIVATE IMPLEMENTATION
 */

class ApplicationPrivate {
    Q_DECLARE_PUBLIC(Application)

 public:
    explicit ApplicationPrivate(Application* parent)
        : q_ptr(parent) {
    }

    void exit(int returnCode) {
        // get the application instance and exit
        QCoreApplication* app = QCoreApplication::instance();
        app->exit(returnCode);
    }

    QStringList arguments() {
        return QCoreApplication::arguments();
    }

 private:
    Application* q_ptr;
};

/*
 * PUBLIC IMPLEMENTATION
 */


Application::Application(QObject *parent)
    : QObject(parent),
      d_ptr(new ApplicationPrivate(this)) {
}

void
Application::exit(int returnCode) {
    Q_D(Application);
    d->exit(returnCode);
}

QStringList
Application::arguments() {
    Q_D(Application);
    return d->arguments();
}

}  // System

}  // DownloadManager

}  // Ubuntu
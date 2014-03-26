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

#ifndef TEST_STOP_REQUEST_TRANSITION_H
#define TEST_STOP_REQUEST_TRANSITION_H

#include <QFinalState>
#include <QObject>
#include <QState>
#include <QStateMachine>
#include <ubuntu/downloads/state_machines/download_sm.h>
#include "base_testcase.h"
#include "sm_file_download.h"

using namespace Ubuntu::DownloadManager::Daemon::StateMachines;

class TestStopRequestTransition : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestStopRequestTransition(QObject *parent = 0)
        : BaseTestCase("TestStopRequestTransition", parent) {}

 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;
    void testOnTransition();

 private:
    QStateMachine _stateMachine;
    MockSMFileDownload* _down;
    PauseRequestTransition* _transition;
    QState* _s1;
    QFinalState* _s2;
};

#endif // TEST_STOP_REQUEST_TRANSITION_H

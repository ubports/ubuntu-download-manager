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

#ifndef TEST_FINAL_STATE_H
#define TEST_FINAL_STATE_H

#include <QObject>
#include <QState>
#include <QStateMachine>
#include <downloads/state_machines/final_state.h>
#include "base_testcase.h"
#include "test_runner.h"


using namespace Ubuntu::DownloadManager::StateMachines;

class TestFinalStateHelperObject : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString surname READ surname WRITE setSurname)

 public:
    explicit TestFinalStateHelperObject(QObject* parent=0)
        : QObject(parent),
          _name(""),
          _surname("") {
    }

    TestFinalStateHelperObject(QString name,
                               QString surname,
                               QObject* parent=0)
        : QObject(parent),
          _name(name),
          _surname(surname) {
    }

    QString name() {
        return _name;
    }

    void setName(QString name) {
        _name = name;
    }

    QString surname() {
        return _surname;
    }

    void setSurname(QString surname) {
        surname = _surname;
    }

 private:
    QString _name = "";
    QString _surname = "";
};

class TestFinalState : public BaseTestCase {
    Q_OBJECT
public:
    explicit TestFinalState(QObject *parent = 0);
    
 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;
    void testSetNotProperties();
    void testSetSingleProperty();
    void testMultipleProperties();

 private:
    TestFinalStateHelperObject* _obj;
    QStateMachine _stateMachine;
    QState* _s1;
    FinalState* _s2;
};

DECLARE_TEST(TestFinalState)

#endif // TEST_FINAL_STATE_H

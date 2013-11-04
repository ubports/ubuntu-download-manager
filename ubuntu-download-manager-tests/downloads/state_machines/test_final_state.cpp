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

#include "test_final_state.h"

TestFinalState::TestFinalState(QObject *parent)
    : BaseTestCase("TestFinalState", parent) {
}

void
TestFinalState::init() {
    BaseTestCase::init();
    _obj = new TestFinalStateHelperObject();
    _s1 = new QState();
    _s2 = new FinalState();

    _stateMachine.addState(_s1);
    _stateMachine.addState(_s2);

    _stateMachine.setInitialState(_s1);
}

void
TestFinalState::cleanup() {
    BaseTestCase::cleanup();
    _stateMachine.removeState(_s1);
    _stateMachine.removeState(_s2);
    delete _obj;
    delete _s1;
    delete _s2;
}

void
TestFinalState::testSetNotProperties() {
    QFAIL("Not implemented");
}

void
TestFinalState::testSetSingleProperty() {
    QFAIL("Not implemented");
}

void
TestFinalState::testMultipleProperties() {
    QFAIL("Not implemented");
}

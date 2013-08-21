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

#include <string>
#include "./fake.h"

/*
 * BOOL WRAPPER
 */


BoolWrapper::BoolWrapper(bool value, QObject* parent)
    : QObject(parent),
      _value(value) {
}

bool
BoolWrapper::value() {
    return _value;
}

void
BoolWrapper::setValue(bool value) {
    _value = value;
}

/*
 * STRING WRAPPER
 */

StringWrapper::StringWrapper(const QString& string, QObject* parent)
    : QObject(parent),
      _value(string) {
}

QString
StringWrapper::value() {
    return _value;
}

void
StringWrapper::setValue(const QString& value) {
    _value = value;
}

/*
 * STRING LIST WRAPPER
 */


StringListWrapper::StringListWrapper(const QStringList& list, QObject* parent)
    : QObject(parent) {
    _value = list;
}

QStringList
StringListWrapper::value() {
    return _value;
}

void
StringListWrapper::setValue(const QStringList& value) {
    _value = value;
}

/*
 * INT WRAPPER
 */

IntWrapper::IntWrapper(int value, QObject* parent)
    : QObject(parent),
      _value(value) {
}


int
IntWrapper::value() {
    return _value;
}

void
IntWrapper::setValue(int value) {
    _value = value;
}

/*
 * UINT WRAPPER
 */

UintWrapper::UintWrapper(uint value, QObject* parent)
    : QObject(parent),
      _value(value) {
}


uint
UintWrapper::value() {
    return _value;
}

void
UintWrapper::setValue(uint value) {
    _value = value;
}

/*
 * METHOD PARAMS
 */

typedef QPair<QList<QObject*>, QList<QObject*> > MethodParamsPair;

MethodParams::MethodParams() : MethodParamsPair() {
}

MethodParams::MethodParams(QList<QObject*> inParams, QList<QObject*> outParams)
    : MethodParamsPair(inParams, outParams) {
}

QList<QObject*>
MethodParams::inParams() {
    return first;
}

void
MethodParams::setInParams(QList<QObject*> params) {
    first = params;
}

QList<QObject*>
MethodParams::outParams() {
    return second;
}

void
MethodParams::setOutParams(QList<QObject*> params) {
    second = params;
}

/*
 * METHOD DATA
 */

typedef QPair<QString, MethodParams> MethodDataPair;

MethodData::MethodData()
    : MethodDataPair() {
}

MethodData::MethodData(const QString& name, const MethodParams& params)
    : MethodDataPair(name, params) {
}

QString
MethodData::methodName() {
    return first;
}

void
MethodData::setMethodName(const QString& name) {
    first = name;
}

MethodParams
MethodData::params() {
    return second;
}

void
MethodData::setParams(const MethodParams& params) {
    second = params;
}

/*
 * FAKE
 */

Fake::Fake()
    : _recording(false) {
}


void
Fake::record() {
    _recording = true;
}

void
Fake::stopRecording() {
    _recording = false;
}

void
Fake::clear() {
    _called.clear();
}

QList<MethodData>
Fake::calledMethods() {
    return _called;
}

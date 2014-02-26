/*
 * Copyright 2014 Canonical Ltd.
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

#include <QDBusArgument>
#include <QProcess>
#include "process_error_struct.h"

namespace Ubuntu {

namespace Errors {

ProcessErrorStruct::ProcessErrorStruct()
    : _code(0),
      _exitCode(0),
      _phrase(""),
      _stdout(""),
      _stderr("") {
}

ProcessErrorStruct::ProcessErrorStruct(int code, QString phrase)
    : _code(code),
      _exitCode(0),
      _phrase(phrase),
      _stdout(""),
      _stderr("") {
}

ProcessErrorStruct::ProcessErrorStruct(int code,
                                       int exitCode,
                                       QString standardOutput,
                                       QString standardError)
    : _code(code),
      _exitCode(exitCode),
      _stdout(standardOutput),
      _stderr(standardError) {

    switch(_code) {
        case QProcess::FailedToStart:
            _phrase = "FailedToStart";
            break;
        case QProcess::Crashed:
            _phrase = "Crashed";
            break;
        case QProcess::Timedout:
            _phrase = "Timedout";
            break;
        case QProcess::WriteError:
            _phrase = "WriteError";
            break;
        case QProcess::ReadError:
            _phrase = "ReadError";
            break;
        default:
            _phrase = "UnknownError";
    }
}

ProcessErrorStruct::ProcessErrorStruct(int code,
                                       QString phrase,
                                       int exitCode,
                                       QString standardOutput,
                                       QString standardError)
    : _code(code),
      _exitCode(exitCode),
      _phrase(phrase),
      _stdout(standardOutput),
      _stderr(standardError) {
}

ProcessErrorStruct::ProcessErrorStruct(const ProcessErrorStruct& other)
    : _code(other._code),
      _exitCode(other._exitCode),
      _phrase(other._phrase),
      _stdout(other._stdout),
      _stderr(other._stderr) {
}

ProcessErrorStruct&
ProcessErrorStruct::operator=(const ProcessErrorStruct& other){
    _code = other._code;
    _exitCode = other._exitCode;
    _phrase = other._phrase;
    _stdout = other._stdout;
    _stderr = other._stderr;

    return *this;
}

QDBusArgument &operator<<(QDBusArgument &argument,
                          const ProcessErrorStruct& error) {
    argument.beginStructure();
    argument << error._code;
    argument << error._phrase;
    argument << error._exitCode;
    argument << error._stdout;
    argument << error._stderr;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument,
                                ProcessErrorStruct& error) {
    argument.beginStructure();
    argument >> error._code;
    argument >> error._phrase;
    argument >> error._exitCode;
    argument >> error._stdout;
    argument >> error._stderr;
    argument.endStructure();

    return argument;
}

int
ProcessErrorStruct::getCode() {
    return _code;
}

int
ProcessErrorStruct::getExitCode() {
    return _exitCode;
}

QString
ProcessErrorStruct::getPhrase() {
    return _phrase;
}

QString
ProcessErrorStruct::getStandardOutput() {
    return _stdout;
}

QString
ProcessErrorStruct::getStandardError() {
    return _stderr;
}


}  // Errors

}  // Ubuntu

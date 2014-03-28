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
#include <QNetworkReply>
#include "network_error_struct.h"

namespace Ubuntu {

namespace Transfers {

namespace Errors {

NetworkErrorStruct::NetworkErrorStruct()
    :  _code(0),
       _phrase("No error condition.") {
}

NetworkErrorStruct::NetworkErrorStruct(int error)
    : _code(error) {

    switch(_code) {
        case QNetworkReply::ConnectionRefusedError:
            _phrase = "ConnectionRefusedError";
            break;
        case QNetworkReply::RemoteHostClosedError:
            _phrase = "RemoteHostClosedError";
            break;
        case QNetworkReply::HostNotFoundError:
            _phrase = "HostNotFoundError";
            break;
        case QNetworkReply::TimeoutError:
            _phrase = "TimeoutError";
            break;
        case QNetworkReply::OperationCanceledError:
            _phrase = "OperationCanceledError";
            break;
        case QNetworkReply::SslHandshakeFailedError:
            _phrase = "SslHandshakeFailedError";
            break;
        case QNetworkReply::TemporaryNetworkFailureError:
            _phrase = "TemporaryNetworkFailureError";
            break;
        case QNetworkReply::NetworkSessionFailedError:
            _phrase = "NetworkSessionFailedError";
            break;
        case QNetworkReply::BackgroundRequestNotAllowedError:
            _phrase = "BackgroundRequestNotAllowedError";
            break;
        case QNetworkReply::ProxyConnectionRefusedError:
            _phrase = "ProxyConnectionRefusedError";
            break;
        case QNetworkReply::ProxyConnectionClosedError:
            _phrase = "ProxyConnectionClosedError";
            break;
        case QNetworkReply::ProxyNotFoundError:
            _phrase = "ProxyNotFoundError";
            break;
        case QNetworkReply::ProxyTimeoutError:
            _phrase = "ProxyTimeoutError";
            break;
        case QNetworkReply::ProxyAuthenticationRequiredError:
            _phrase = "ProxyAuthenticationRequiredError";
            break;
        case QNetworkReply::ContentAccessDenied:
            _phrase = "ContentAccessDenied";
            break;
        case QNetworkReply::ContentOperationNotPermittedError:
            _phrase = "ContentOperationNotPermittedError";
            break;
        case QNetworkReply::ContentNotFoundError:
            _phrase = "ContentNotFoundError";
            break;
        case QNetworkReply::AuthenticationRequiredError:
            _phrase = "AuthenticationRequiredError";
            break;
        case QNetworkReply::ContentReSendError:
            _phrase = "ContentReSendError";
            break;
        case QNetworkReply::ProtocolUnknownError:
            _phrase = "ProtocolUnknownError";
            break;
        case QNetworkReply::ProtocolInvalidOperationError:
            _phrase = "ProtocolInvalidOperationError";
            break;
        case QNetworkReply::UnknownNetworkError:
            _phrase = "UnknownNetworkError";
            break;
        case QNetworkReply::UnknownProxyError:
            _phrase = "UnknownProxyError";
            break;
        case QNetworkReply::UnknownContentError:
            _phrase = "UnknownContentError";
            break;
        case QNetworkReply::ProtocolFailure:
            _phrase = "ProtocolFailure";
            break;
        default:
            _phrase = "UnknownError";
        }
}

NetworkErrorStruct::NetworkErrorStruct(int code, QString phrase)
    : _code(code),
      _phrase(phrase) {
}

NetworkErrorStruct::NetworkErrorStruct(const NetworkErrorStruct& other)
    : _code(other._code),
      _phrase(other._phrase) {
}

NetworkErrorStruct&
NetworkErrorStruct::operator=(const NetworkErrorStruct& other) {
    _code = other._code;
    _phrase = other._phrase;

    return *this;
}

QDBusArgument &operator<<(QDBusArgument &argument,
                          const NetworkErrorStruct& error) {
    argument.beginStructure();
    argument << error._code;
    argument << error._phrase;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument,
                                NetworkErrorStruct& error) {
    argument.beginStructure();
    argument >> error._code;
    argument >> error._phrase;
    argument.endStructure();

    return argument;
}

int
NetworkErrorStruct::getCode() const {
    return _code;
}

QString
NetworkErrorStruct::getPhrase() const {
    return _phrase;
}

}  // Errors

}  // Transfers

}  // Ubuntu

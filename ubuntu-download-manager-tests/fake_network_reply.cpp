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

#include "./fake_qnetwork_reply.h"
#include "./fake_network_reply.h"

FakeNetworkReply::FakeNetworkReply(QObject *parent)
    : NetworkReply(new FakeQNetworkReply(parent)),
      Fake() {
}

QByteArray
FakeNetworkReply::data() {
    return _data;
}

void
FakeNetworkReply::setData(QByteArray data) {
    _data = data;
}

QByteArray
FakeNetworkReply::readAll() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("readAll");
        _called.append(methodData);
    }
    return _data;
}

void
FakeNetworkReply::abort() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("abort");
        _called.append(methodData);
    }
}

void
FakeNetworkReply::setReadBufferSize(uint size) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new UintWrapper(size));

        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("setReadBufferSize", params);
        _called.append(methodData);
    }
}

void
FakeNetworkReply::emitFinished() {
    emit finished();
}

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

#include <QSslError>
#include <QSignalSpy>
#include "fakes/fake_qnetwork_reply.h"
#include "test_network_reply.h"

TestNetworkReply::TestNetworkReply(QObject *parent)
    : BaseTestCase("TestNetworkReply", parent) {
}

void
TestNetworkReply::init() {
    BaseTestCase::init();
    _qReply = new FakeQNetworkReply();
    _reply = new NetworkReply(_qReply);
}

void
TestNetworkReply::cleanup() {
    BaseTestCase::cleanup();

    if (_reply != NULL)
        _reply->deleteLater();

    if (_qReply != NULL)
        _qReply->deleteLater();
}

void
TestNetworkReply::testDownloadProgressForwarded_data() {
    QTest::addColumn<uint>("received");
    QTest::addColumn<uint>("total");

    QTest::newRow("First row") << 67u << 200u;
    QTest::newRow("Second row") << 45u << 12000u;
    QTest::newRow("Third row") << 2u << 2345u;
    QTest::newRow("Last row") << 3434u << 2323u;
}

void
TestNetworkReply::testDownloadProgressForwarded() {
    QFETCH(uint, received);
    QFETCH(uint, total);

    QSignalSpy spy(_reply, SIGNAL(downloadProgress(qint64, qint64)));
    emit _qReply->downloadProgress(received, total);

    QTRY_COMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toUInt(), received);
    QCOMPARE(arguments.at(1).toUInt(), total);
}

void
TestNetworkReply::testErrorForwarded() {
    QSignalSpy spy(_reply, SIGNAL(error(QNetworkReply::NetworkError)));
    emit _qReply->error(QNetworkReply::NoError);

    QTRY_COMPARE(spy.count(), 1);
}

void
TestNetworkReply::testFinishedForwarded() {
    QSignalSpy spy(_reply, SIGNAL(finished()));
    emit _qReply->finished();
    QTRY_COMPARE(spy.count(), 1);
}

void
TestNetworkReply::testSslErrorsForwarded() {
    QList<QSslError> errors;
    QSignalSpy spy(_reply, SIGNAL(sslErrors(const QList<QSslError>&)));
    emit _qReply->sslErrors(errors);

    QTRY_COMPARE(spy.count(), 1);
}

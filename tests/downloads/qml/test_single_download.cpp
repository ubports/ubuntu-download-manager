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

#include <QScopedPointer>

#include <gmock/gmock.h>

#include <matchers.h>
#include "error.h"
#include "test_single_download.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Return;

TestSingleDownload::TestSingleDownload(QObject *parent)
    : BaseTestCase("TestSingleDownload", parent) {
}

void
TestSingleDownload::init() {
    BaseTestCase::init();

    _metadata["my-string"] = "canonical";
    _metadata["your-string"] = "developer";
    _headers["test"] = "test";
    _headers["my test"] = "my test";
    _url = "http://example.com";

    _down = new MockDownload();
    _man = new MockManager();
}

void
TestSingleDownload::cleanup() {
    BaseTestCase::cleanup();
    delete _down;
    delete _man;
}

void
TestSingleDownload::verifyMocks() {
    QVERIFY(Mock::VerifyAndClearExpectations(_down));
    QVERIFY(Mock::VerifyAndClearExpectations(_man));
}

void
TestSingleDownload::testCanceledSignalFwd() {
    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(_down, _man));

    SignalBarrier spy(singleDownload.data(), SIGNAL(canceled(bool)));
    _down->canceled(true);

    QVERIFY(spy.ensureSignalEmitted());

    verifyMocks();
}

void
TestSingleDownload::testFinishedSingalFwd() {
    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(_down, _man));

    SignalBarrier spy(singleDownload.data(), SIGNAL(finished(const QString&)));
    _down->finished(QString("TEST"));

    QVERIFY(spy.ensureSignalEmitted());

    verifyMocks();
}

void
TestSingleDownload::testPausedSignalFwd() {
    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(_down, _man));

    SignalBarrier spy(singleDownload.data(), SIGNAL(paused(bool)));
    _down->paused(true);

    QVERIFY(spy.ensureSignalEmitted());

    verifyMocks();
}

void
TestSingleDownload::testProcessingSignalFwd() {
    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(_down, _man));

    SignalBarrier spy(singleDownload.data(), SIGNAL(processing(const QString&)));
    _down->processing(QString("TEST"));

    QVERIFY(spy.ensureSignalEmitted());

    verifyMocks();
}

void
TestSingleDownload::testProgressSignalFwd() {
    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(_down, _man));

    SignalBarrier spy(singleDownload.data(),
        SIGNAL(progressReceived(qulonglong, qulonglong)));
    _down->progress(30, 700);

    QVERIFY(spy.ensureSignalEmitted());

    verifyMocks();
}

void
TestSingleDownload::testResumedSignalFwd() {
    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(_down, _man));

    SignalBarrier spy(singleDownload.data(), SIGNAL(resumed(bool)));
    _down->resumed(true);

    QVERIFY(spy.ensureSignalEmitted());

    verifyMocks();
}

void
TestSingleDownload::testStartedSignalFwd() {
    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(_down, _man));

    SignalBarrier spy(singleDownload.data(), SIGNAL(started(bool)));
    _down->started(true);

    QVERIFY(spy.ensureSignalEmitted());

    verifyMocks();
}

void
TestSingleDownload::testSetAllowMobileDataNullptr() {
    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(nullptr, _man));

    // ensure that the mocks are not called and we do not crash
    singleDownload->setAllowMobileDownload(true);
    verifyMocks();
}

void
TestSingleDownload::testSetAllowMobileDataError() {
    QScopedPointer<MockError> err(new MockError(Error::DBus));
    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(_down, _man));

    EXPECT_CALL(*_down, allowMobileDownload(true))
        .Times(1);

    EXPECT_CALL(*_down, isError())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_down, error())
        .Times(1)
        .WillOnce(Return(err.data()));

    EXPECT_CALL(*err.data(), errorString())
        .Times(1)
        .WillOnce(Return(QString("My error")));

    // ensure that the diff signals are emitted
    SignalBarrier spy(singleDownload.data(), SIGNAL(errorChanged()));

    singleDownload->setAllowMobileDownload(true);

    QVERIFY(spy.ensureSignalEmitted());
    verifyMocks();
}

void
TestSingleDownload::testSetAllowMobileDataSuccess() {
    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(_down, _man));

    EXPECT_CALL(*_down, allowMobileDownload(true))
        .Times(1);

    EXPECT_CALL(*_down, isError())
        .Times(1)
        .WillOnce(Return(false));

    // ensure that the diff signals are emitted
    SignalBarrier spy(singleDownload.data(), SIGNAL(allowMobileDownloadChanged()));

    singleDownload->setAllowMobileDownload(true);

    QVERIFY(spy.ensureSignalEmitted());
    verifyMocks();
}

void
TestSingleDownload::testSetThrottleNullptr() {
    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(nullptr, _man));

    singleDownload->setThrottle(3);
    // ensure that the mocks are not called and we do not crash
    verifyMocks();
}

void
TestSingleDownload::testSetThrottleError() {
    QScopedPointer<MockError> err(new MockError(Error::DBus));
    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(_down, _man));

    EXPECT_CALL(*_down, setThrottle(9))
        .Times(1);

    EXPECT_CALL(*_down, isError())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_down, error())
        .Times(1)
        .WillOnce(Return(err.data()));

    EXPECT_CALL(*err.data(), errorString())
        .Times(1)
        .WillOnce(Return(QString("My error")));

    // ensure that the diff signals are emitted
    SignalBarrier spy(singleDownload.data(), SIGNAL(errorChanged()));

    singleDownload->setThrottle(9);

    QVERIFY(spy.ensureSignalEmitted());
    verifyMocks();
}

void
TestSingleDownload::testSetThrottleSuccess() {
    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(_down, _man));

    EXPECT_CALL(*_down, setThrottle(9))
        .Times(1);

    EXPECT_CALL(*_down, isError())
        .Times(1)
        .WillOnce(Return(false));

    // ensure that the diff signals are emitted
    SignalBarrier spy(singleDownload.data(), SIGNAL(throttleChanged()));

    singleDownload->setThrottle(9);

    QVERIFY(spy.ensureSignalEmitted());
    verifyMocks();
}

void
TestSingleDownload::testSetHeadersNullptr() {
    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(nullptr, _man));
    QVariantMap map;

    // ensure we do not crash
    singleDownload->setHeaders(map);
    verifyMocks();
}

void
TestSingleDownload::testSetHeadersError() {
    QScopedPointer<MockError> err(new MockError(Error::DBus));
    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(_down, _man));

    QVariantMap map;

    EXPECT_CALL(*_down, setHeaders(_))
        .Times(1);

    EXPECT_CALL(*_down, isError())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_down, error())
        .Times(1)
        .WillOnce(Return(err.data()));

    EXPECT_CALL(*err.data(), errorString())
        .Times(1)
        .WillOnce(Return(QString("My error")));

    // ensure that the diff signals are emitted
    SignalBarrier spy(singleDownload.data(), SIGNAL(errorChanged()));

    singleDownload->setHeaders(map);

    QVERIFY(spy.ensureSignalEmitted());
    verifyMocks();
}

void
TestSingleDownload::testSetHeadersSuccess() {
    QString header1, value1, header2, value2;
    header1 = "my-first-header";
    value1 = "first";
    header2 = "second-header";
    value2 = "second";

    QVariantMap map;
    map[header1] = value1;
    map[header2] = value2;

    QMap<QString, QString> headers;
    headers[header1] = value1;
    headers[header2] = value2;

    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(_down, _man));

    EXPECT_CALL(*_down, setHeaders(QStringMapEq(headers)))
        .Times(1);

    EXPECT_CALL(*_down, isError())
        .Times(1)
        .WillOnce(Return(false));

    // ensure that the diff signals are emitted
    SignalBarrier spy(singleDownload.data(), SIGNAL(headersChanged()));

    singleDownload->setHeaders(map);

    QVERIFY(spy.ensureSignalEmitted());
    verifyMocks();
}

void
TestSingleDownload::testSetMetadataNullptr() {
    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(nullptr, _man));
    auto metadata = new Metadata();

    // ensure we do not crash
    singleDownload->setMetadata(metadata);
    verifyMocks();
}

void
TestSingleDownload::testSetMetadataToNullptr() {
    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(_down, _man));

    // ensure we do not crash
    singleDownload->setMetadata(nullptr);
    verifyMocks();
}

void
TestSingleDownload::testSetMetadataError() {
    QScopedPointer<MockError> err(new MockError(Error::DBus));
    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(_down, _man));

    auto metadata = new Metadata();

    EXPECT_CALL(*_down, setMetadata(_))
        .Times(1);

    EXPECT_CALL(*_down, isError())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_down, error())
        .Times(1)
        .WillOnce(Return(err.data()));

    EXPECT_CALL(*err.data(), errorString())
        .Times(1)
        .WillOnce(Return(QString("My error")));

    // ensure that the diff signals are emitted
    SignalBarrier spy(singleDownload.data(), SIGNAL(errorChanged()));

    singleDownload->setMetadata(metadata);

    QVERIFY(spy.ensureSignalEmitted());
    verifyMocks();
}

void
TestSingleDownload::testSetMetadataSuccess() {
    auto metadata = new Metadata();
    metadata->setTitle("My download");
    metadata->setShowInIndicator(true);

    QScopedPointer<TestableSingleDownload> singleDownload(
        new TestableSingleDownload(_down, _man));

    EXPECT_CALL(*_down, setMetadata(MetadataEq(metadata)))
        .Times(1);

    EXPECT_CALL(*_down, isError())
        .Times(1)
        .WillOnce(Return(false));

    // ensure that the diff signals are emitted
    SignalBarrier spy(singleDownload.data(), SIGNAL(metadataChanged()));

    singleDownload->setMetadata(metadata);

    QVERIFY(spy.ensureSignalEmitted());
    verifyMocks();
}

QTEST_MAIN(TestSingleDownload)
#include "moc_test_single_download.cpp"

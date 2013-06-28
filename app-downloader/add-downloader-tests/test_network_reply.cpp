#include <QSslError>
#include <QSignalSpy>
#include "test_network_reply.h"
#include "fake_qnetwork_reply.h"

TestNetworkReply::TestNetworkReply(QObject *parent) :
    QObject(parent)
{
}

void TestNetworkReply::init()
{
    _qReply = new FakeQNetworkReply();
    _reply = new NetworkReply(_qReply);
}

void TestNetworkReply::cleanup()
{
    if (_reply != NULL)
        _reply->deleteLater();

    if (_qReply != NULL)
        _qReply->deleteLater();
}

void TestNetworkReply::testDownloadProgressForwarded_data()
{
    QTest::addColumn<uint>("received");
    QTest::addColumn<uint>("total");

    QTest::newRow("First row") << 67u << 200u;
    QTest::newRow("Second row") << 45u << 12000u;
    QTest::newRow("Third row") << 2u << 2345u;
    QTest::newRow("Last row") << 3434u << 2323u;
}

void TestNetworkReply::testDownloadProgressForwarded()
{
    QFETCH(uint, received);
    QFETCH(uint, total);

    QSignalSpy spy(_reply, SIGNAL(downloadProgress(qint64, qint64)));
    emit _qReply->downloadProgress(received, total);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toUInt(), received);
    QCOMPARE(arguments.at(1).toUInt(), total);
}

void TestNetworkReply::testErrorForwarded()
{
    QSignalSpy spy(_reply, SIGNAL(error(QNetworkReply::NetworkError)));
    emit _qReply->error(QNetworkReply::NoError);

    QCOMPARE(spy.count(), 1);
}

void TestNetworkReply::testFinishedForwarded()
{
    QSignalSpy spy(_reply, SIGNAL(finished()));
    emit _qReply->finished();
    QCOMPARE(spy.count(), 1);
}

void TestNetworkReply::testSslErrorsForwarded()
{
    QList<QSslError> errors;
    QSignalSpy spy(_reply, SIGNAL(sslErrors(const QList<QSslError>&)));
    emit _qReply->sslErrors(errors);

    QCOMPARE(spy.count(), 1);
}

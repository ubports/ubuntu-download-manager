#ifndef FAKE_QNETWORK_ACCESS_MANAGER_H
#define FAKE_QNETWORK_ACCESS_MANAGER_H

#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QList>
#include <QPair>
#include "fake.h"

class RequestWrapper : public QObject
{
    Q_OBJECT

public:
    explicit RequestWrapper(const QNetworkRequest& request, QObject* parent=0);

    QNetworkRequest request();
private:
    QNetworkRequest _request;
};

class FakeQNetworkAccessManager : public QNetworkAccessManager, public Fake
{
    Q_OBJECT

public:
    explicit FakeQNetworkAccessManager(QObject* parent = 0);

    // overriden methods used to fake the nam
    QNetworkReply* get(const QNetworkRequest& request);
};

#endif // FAKE_QNETWORK_ACCESS_MANAGER_H

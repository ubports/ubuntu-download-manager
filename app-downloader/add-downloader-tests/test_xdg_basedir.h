#ifndef TEST_XDG_BASEDIR_H
#define TEST_XDG_BASEDIR_H

#include <functional>
#include <QHash>
#include <QObject>
#include "xdg_basedir.h"
#include "test_runner.h"

class TestXDGBasedir : public QObject
{
    Q_OBJECT
public:
    explicit TestXDGBasedir(QObject *parent = 0);

private slots:

    void init();
    void cleanup();
    void testCacheHomeDefault();
    void testCacheHomeEnv();
    void testConfigHomeDefault();
    void testConfigHomeEnv();
    void testDataHomeDefault();
    void testDataHomeEnv();
    void testConfigDirsDefault();
    void testConfigEnv();
    void testDataDirsDefault();
    void testDataDirsEnv();

private:

    void testDefault(QString envVar, std::function<QString(void)> callBack, QString expected);
    void testUserSet(QString envVar, std::function<QString(void)> cb, QString expected);
    void testCollectionDefault(QString envVar, std::function<QList<QString>()> cb,
        QString expected);
    void testCollectionEnv(QString envVar, std::function<QList<QString>()> cb,
        QList<QString> expected);
    QHash<QString, QString> _envData;
};

DECLARE_TEST(TestXDGBasedir)

#endif // TEST_XDG_BASEDIR_H

#include "test_xdg_basedir.h"

#include "test_xdg_basedir.h"
#include <stdlib.h>
#include <functional>
#include <QDir>


TestXDGBasedir::TestXDGBasedir(QObject *parent) :
    QObject(parent)
{
}

void TestXDGBasedir::init()
{
    QList<QString> envVars;
    envVars << "XDG_CACHE_HOME" << "XDG_CONFIG_HOME" << "XDG_DATA_HOME"
        << "XDG_CONFIG_DIRS" << "XDG_DATA_DIRS";

    // load all the env data in the has to be later set
    foreach(QString var, envVars)
    {
        char* env_path = getenv(var.toStdString().c_str());
        if (env_path == NULL)
        {
            _envData[var] = "NULL";
        }
        _envData[var] = QString(env_path);
        // clean the data
        setenv(var.toStdString().c_str(), NULL, 1);
    }
}

void TestXDGBasedir::cleanup()
{
    // set env data back
    QList<QString> keys = _envData.keys();
    foreach(QString key, keys)
    {
        QString data = _envData[key];
        if (data == "NULL")
        {
            setenv(key.toStdString().c_str(), NULL, 1);
        }
        else
        {
            setenv(key.toStdString().c_str(), data.toStdString().c_str(), 1);
        }
    }
}

void TestXDGBasedir::testDefault(QString envVar, std::function<QString(void)> callBack, QString expected)
{
    setenv(envVar.toStdString().c_str(), NULL, 1);
    QString result = callBack();
    QCOMPARE(expected, result);
}

void TestXDGBasedir::testUserSet(QString envVar, std::function<QString(void)> cb, QString expected)
{
    setenv(envVar.toStdString().c_str(), expected.toStdString().c_str(), 1);
    QString result = cb();
    QCOMPARE(expected, result);
}

void TestXDGBasedir::testCacheHomeDefault()
{
    QString expected = QDir(QDir::homePath()).filePath(".cache");
    std::function<QString(void)> cb = [](){return XDGBasedir::cacheHome();};
    testDefault(QString("XDG_CACHE_HOME"), cb, expected);
}

void TestXDGBasedir::testCacheHomeEnv()
{
    QString expected = "/path/to/cache";
    std::function<QString(void)> cb = [](){return XDGBasedir::cacheHome();};
    testUserSet(QString("XDG_CACHE_HOME"), cb, expected);
}

void TestXDGBasedir::testConfigHomeDefault()
{
    QString expected = QDir(QDir::homePath()).filePath(".config");
    std::function<QString(void)> cb = [](){return XDGBasedir::configHome();};
    testDefault(QString("XDG_CONFIG_HOME"), cb, expected);
}

void TestXDGBasedir::testConfigHomeEnv()
{
    QString expected = "/path/to/config/home";
    std::function<QString(void)> cb = [](){return XDGBasedir::configHome();};
    testUserSet(QString("XDG_CONFIG_HOME"), cb, expected);
}

void TestXDGBasedir::testDataHomeDefault()
{
    QString expected = QDir(QDir::homePath()).filePath(".local/share");
    std::function<QString(void)> cb = [](){return XDGBasedir::dataHome();};
    testDefault(QString("XDG_DATA_HOME"), cb, expected);
}

void TestXDGBasedir::testDataHomeEnv()
{
    QString expected = "/path/to/data";
    std::function<QString(void)> cb = [](){return XDGBasedir::dataHome();};
    testUserSet(QString("XDG_DATA_HOME"), cb, expected);
}

void TestXDGBasedir::testCollectionDefault(QString envVar, std::function<QList<QString>()> cb,
    QString expected)
{
    setenv(envVar.toStdString().c_str(), NULL, 1);
    QList<QString> dirs = cb();
    QVERIFY(1 == dirs.length());
    qDebug() << expected;
    qDebug() << dirs;
    QVERIFY(dirs.contains(expected));
}

void TestXDGBasedir::testCollectionEnv(QString envVar, std::function<QList<QString>()> cb,
    QList<QString> expected)
{
    QStringList list = QStringList(expected);
    QString envData = list.join(":");
    setenv(envVar.toStdString().c_str(), envData.toStdString().c_str(), 1);
    QList<QString> dirs = cb();
    QCOMPARE(dirs.length(), expected.length() + 1);  // add +1 for the default
    foreach(QString data, expected)
    {
        QVERIFY(dirs.contains(data));
    }
}

void TestXDGBasedir::testConfigDirsDefault()
{
    QString envVar = "XDG_CONFIG_DIRS";
    std::function<QList<QString>()> cb = [](){return XDGBasedir::configDirs();};
    QString expected = XDGBasedir::configHome();
    testCollectionDefault(envVar, cb, expected);
}

void TestXDGBasedir::testConfigEnv()
{
    QString envVar = "XDG_CONFIG_DIRS";
    std::function<QList<QString>()> cb = [](){return XDGBasedir::configDirs();};
    QList<QString> expected;
    expected.append(QString("/path/to/config1"));
    expected.append(QString("/path/to/config2"));
    expected.append(QString("/path/to/config3"));
    testCollectionEnv(envVar, cb, expected);
}

void TestXDGBasedir::testDataDirsDefault()
{
    QString envVar = "XDG_DATA_DIRS";
    std::function<QList<QString>()> cb = [](){return XDGBasedir::dataDirs();};
    QString expected = XDGBasedir::dataHome();
    testCollectionDefault(envVar, cb, expected);
}

void TestXDGBasedir::testDataDirsEnv()
{
    QString envVar = "XDG_DATA_DIRS";
    std::function<QList<QString>()> cb = [](){return XDGBasedir::dataDirs();};
    QList<QString> expected;
    expected.append(QString("/path/to/data1"));
    expected.append(QString("/path/to/data2"));
    expected.append(QString("/path/to/data3"));
    testCollectionEnv(envVar, cb, expected);
}

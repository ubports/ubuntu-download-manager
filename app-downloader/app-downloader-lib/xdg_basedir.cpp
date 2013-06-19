#include "xdg_basedir.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <stdlib.h>

// definitions of defaults
#define DEFAULT_CACHE_HOME ".cache"
#define DEFAULT_CONFIG_PATH "/etc/xdg"
#define DEFAULT_CONFIG_HOME ".config"
#define DEFAULT_DATA_PATH "/usr/local/share:/usr/share"
#define DEFAULT_DATA_HOME ".local/share"
#define XDG_CACHE_HOME "XDG_CACHE_HOME"
#define XDG_CONFIG_HOME "XDG_CONFIG_HOME"
#define XDG_DATA_HOME "XDG_DATA_HOME"
#define XDG_CONFIG_DIRS "XDG_CONFIG_DIRS"
#define XDG_DATA_DIRS "XDG_DATA_DIRS"

QString XDGBasedir::cacheHome()
{
    qDebug() << "XDGBasedir::cacheHome()";
    QString env_path = QString::fromUtf8(getenv(XDG_CACHE_HOME)).trimmed();
    if (env_path.isEmpty())
    {
        QString defaultValue = QDir(QDir::homePath()).filePath(DEFAULT_CACHE_HOME);
        qDebug() << "NULL" << QString::fromStdString(XDG_CACHE_HOME) << "Returning default" << defaultValue;
        return defaultValue;
    }
    return env_path;
}

QString XDGBasedir::configHome()
{
    qDebug() << "XDGBasedir::configHome()";
    QString env_path = QString::fromUtf8(getenv(XDG_CONFIG_HOME)).trimmed();
    if (env_path.isEmpty())
    {
        QString defaultValue = QDir(QDir::homePath()).filePath(DEFAULT_CONFIG_HOME);
        qDebug() << "NULL" << QString::fromStdString(XDG_CONFIG_HOME) << "Returning default" << defaultValue;
        return defaultValue;
    }
    return env_path;
}

QString XDGBasedir::dataHome()
{
    qDebug() << "XDGBasedir::dataHome()";
    QString env_path = QString::fromUtf8(getenv(XDG_DATA_HOME)).trimmed();
    if (env_path.isEmpty())
    {
        QString defaultValue = QDir(QDir::homePath()).filePath(DEFAULT_DATA_HOME);
        qDebug() << "NULL" << QString::fromStdString(XDG_DATA_HOME) << "Returning default" << defaultValue;
        return defaultValue;
    }
    return env_path;
}

QList<QString> XDGBasedir::configDirs()
{
    qDebug() << "XDGBasedir::configDirs()";
    QList<QString> result;
    result.append(XDGBasedir::configHome());

    QString env_path = QString::fromUtf8(getenv(XDG_CONFIG_DIRS)).trimmed();
    if (env_path.isEmpty())
    {
        qDebug() << "NULL" << QString::fromStdString(XDG_CONFIG_DIRS);
        return result;
    }

    QStringList paths = env_path.split(":");
    result.append(paths);

    return result;
}

QList<QString> XDGBasedir::dataDirs()
{
    qDebug() << "XDGBasedir::dataDirs()";
    QList<QString> result;
    result.append(XDGBasedir::dataHome());

    QString env_path = QString::fromUtf8(getenv(XDG_DATA_DIRS)).trimmed();
    if (env_path.isEmpty())
    {
        qDebug() << "NULL" << QString::fromStdString(XDG_DATA_DIRS);
        return result;
    }

    QStringList paths = env_path.split(":");
    result.append(paths);
    return result;
}

QList<QString> XDGBasedir::configPaths(QList<QString> resources)
{
    qDebug() << "XDGBasedir::configPaths";
    QList<QString> result;
    QList<QString> configDirs = XDGBasedir::configDirs();
    foreach (QString configDir, configDirs)
    {
        QStringList pathComponents;
        pathComponents.append(configDir);
        pathComponents.append(resources);
        QString path = pathComponents.join(QDir::separator());
        if (QFile::exists(path))
        {
            qDebug() << "Adding new path:" << path;
            result.append(path);
        }
    }

    return result;
}

QList<QString> XDGBasedir::dataPaths(QList<QString> resources)
{
    qDebug() << "XDGBasedir::dataPaths";
    QList<QString> result;
    QList<QString> dataDirs = XDGBasedir::dataDirs();
    foreach (QString dataDir, dataDirs)
    {
        QStringList pathComponents;
        pathComponents.append(dataDir);
        pathComponents.append(resources);
        QString path = pathComponents.join(QDir::separator());
        if (QFile::exists(path))
        {
            qDebug() << "Adding new path:" << path;
            result.append(path);
        }
    }
    return result;
}

QString XDGBasedir::loadFirstConfig(QList<QString> resource)
{
    qDebug() << "XDGBasedir::loadFirstConfig()";
    QList<QString> configPaths = XDGBasedir::configPaths(resource);
    qDebug() << "Config paths length" << configPaths.length();
    return configPaths.at(0);
}

QString XDGBasedir::saveConfigPath(QList<QString> resources)
{
    qDebug() << "XDGBasedir::saveConfigPath()";
    QString home = XDGBasedir::configHome();
    QStringList pathComponents;
    pathComponents.append(home);
    pathComponents.append(resources);
    QString path = pathComponents.join(QDir::separator());
    QFileInfo info = QFileInfo(path);
    if (!info.exists() || !info.isDir())
    {
        QDir dir(home);
        QStringList child;
        child.append(resources);
        bool created = dir.mkpath(child.join(QDir::separator()));
        if (!created)
        {
            qCritical() << "Could not create path:" << child.join(QDir::separator());
            return QString("");
        }
    }
    qDebug() << "Path returned" << path;
    return path;
}

QString XDGBasedir::saveDataPath(QList<QString> resources)
{
    qDebug() << "XDGBasedir::saveDataPath()";
    QString home = XDGBasedir::dataHome();
    QStringList pathComponents;
    pathComponents.append(home);
    pathComponents.append(resources);
    QString path = pathComponents.join(QDir::separator());
    qDebug() << "Path is" << path;
    QFileInfo info = QFileInfo(path);
    if (!info.exists() || !info.isDir())
    {
        QDir dir(home);
        QStringList child;
        child.append(resources);
        bool created = dir.mkpath(child.join(QDir::separator()));
        if (!created)
        {
            qCritical() << "Could not create path:" << child.join(QDir::separator());
            return QString("");
        }
    }
    qDebug() << "Path returned" << path;
    return path;
}

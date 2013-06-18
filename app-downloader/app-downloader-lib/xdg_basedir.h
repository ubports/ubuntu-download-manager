#ifndef APP_DOWNLOADER_LIB_XDG_BASEDIR_H
#define APP_DOWNLOADER_LIB_XDG_BASEDIR_H

#include <QList>
#include <QObject>
#include <QString>
#include <stdio.h>

class XDGBasedir : public QObject
{
    Q_OBJECT
public:

    static QString cacheHome();
    static QString configHome();
    static QString dataHome();
    static QList<QString> configDirs();
    static QList<QString> dataDirs();
    static QList<QString> configPaths(QList<QString> resource);
    static QList<QString> dataPaths(QList<QString> resource);
    static QString loadFirstConfig(QList<QString> resource);
    static QString saveConfigPath(QList<QString> resource);
    static QString saveDataPath(QList<QString> resource);

private:
    static QString DEFAULT_CACHE_HOME;
    static QString DEFAULT_CONFIG_HOME;
    static QString DEFAULT_CONFIG_PATH;
    static QString DEFAULT_DATA_PATH;
    static QString DEFAULT_DATA_HOME;
    static std::string XDG_CACHE_HOME;
    static std::string XDG_CONFIG_HOME;
    static std::string XDG_DATA_HOME;
    static std::string XDG_CONFIG_DIRS;
    static std::string XDG_DATA_DIRS;

};

#endif // APP_DOWNLOADER_LIB_XDG_BASEDIR_H

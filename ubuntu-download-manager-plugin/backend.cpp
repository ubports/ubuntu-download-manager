#include <QtQml>
#include <QtQml/QQmlContext>
#include "backend.h"
#include "download_error.h"
#include "single_download.h"
#include "ubuntu_download_manager.h"


void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("UbuntuDownloadManager"));

    qmlRegisterType<Ubuntu::DownloadManager::DownloadError>(uri, 0, 1, "Error");
    qmlRegisterType<Ubuntu::DownloadManager::SingleDownload>(uri, 0, 1, "SingleDownload");
    qmlRegisterType<Ubuntu::DownloadManager::UbuntuDownloadManager>(uri, 0, 1, "DownloadManager");
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
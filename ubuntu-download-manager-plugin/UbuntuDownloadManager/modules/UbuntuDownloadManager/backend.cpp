#include <QtQml>
#include <QtQml/QQmlContext>
#include "backend.h"
#include "download_error.h"
#include "single_download.h"
#include "ubuntu_download_manager.h"


void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("UbuntuDownloadManager"));

    qmlRegisterType<Ubuntu::DownloadManager::DownloadError>(uri, 1, 0, "Error");
    qmlRegisterType<Ubuntu::DownloadManager::SingleDownload>(uri, 1, 0, "SingleDownload");
    qmlRegisterType<Ubuntu::DownloadManager::UbuntuDownloadManager>(uri, 1, 0, "UbuntuDownloadManager");
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}

#include "system/apn_request_factory.h"
#include "mms_file_download.h"

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

MmsFileDownload::MmsFileDownload(const QString& id,
                    const QString& path,
                    bool isConfined,
                    const QString& rootPath,
                    const QUrl& url,
                    const QVariantMap& metadata,
                    const QMap<QString, QString>& headers,
                    QObject* parent)
    : FileDownload(id, path, isConfined, rootPath, url,
                   metadata, headers, parent){
    _requestFactory = ApnRequestFactory::instance();
    setAddToQueue(false);
}

}  // Daemon

}  // DownloadManager

}  // Ubuntu

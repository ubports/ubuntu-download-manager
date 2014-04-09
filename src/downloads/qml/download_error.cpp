#include "download_error.h"

namespace Ubuntu {

namespace DownloadManager {

DownloadError::DownloadError(QObject *parent) :
    QObject(parent),
    m_message(""),
    m_type("")
{
}

}
}

#ifndef APP_DOWNLOADER_LIB_REQUEST_FACTORY_H
#define APP_DOWNLOADER_LIB_REQUEST_FACTORY_H

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include "app-downloader-lib_global.h"

class RequestFactoryPrivate;
class APPDOWNLOADERLIBSHARED_EXPORT RequestFactory : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RequestFactory)

public:
    explicit RequestFactory(QObject *parent = 0);
    
    virtual QNetworkReply* get(const QNetworkRequest& request);

private:
    // use pimpl so that we can mantains ABI compatibility
    RequestFactoryPrivate* d_ptr;
};

#endif // REQUEST_FACTORY_H

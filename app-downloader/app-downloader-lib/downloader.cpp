#include <QUuid>
#include <QRegExp>
#include <QNetworkAccessManager>
#include <QSignalMapper>
#include "application_download_adaptor.h"
#include "downloader.h"

/**
 * PRIVATE IMPLEMENATION
 */

class DownloaderPrivate
{
    Q_DECLARE_PUBLIC(Downloader)
public:
    explicit DownloaderPrivate(QDBusConnection connection, Downloader* parent);

private:
    QString buildDownloadPath();
    AppDownload* getApplication(QUrl url);
    AppDownload* getApplication(QUrl url, QString hash, QCryptographicHash::Algorithm algo);
    void updateCurrentDownload();
    void onDownloadStateChanged();

    QDBusObjectPath createDownload(const QString &url);
    QDBusObjectPath createDownloadWithHash(const QString &url, const QString &hash, QCryptographicHash::Algorithm algo);
    QList<QDBusObjectPath> getAllDownloads();

private:
    static QString BASE_ACCOUNT_URL;

    QList<AppDownload*> _downloads;
    QHash<QString, ApplicationDownloadAdaptor*> _adaptors;
    QDBusConnection _conn;
    Downloader* q_ptr;
    QNetworkAccessManager* _nam;
    AppDownload* _current;
};

QString DownloaderPrivate::BASE_ACCOUNT_URL = "/com/canonical/applications/download/%1";

DownloaderPrivate::DownloaderPrivate(QDBusConnection connection, Downloader* parent):
    _conn(connection),
    q_ptr(parent)
{
    _nam = new QNetworkAccessManager();
    _current = NULL;
}

QString DownloaderPrivate::buildDownloadPath()
{
    // use a uuid to uniquely identify the downloader
    QUuid uuid = QUuid::createUuid();
    QString uuidString = uuid.toString().replace(QRegExp("[-{}]"), "");
    return DownloaderPrivate::BASE_ACCOUNT_URL.arg(uuidString);
}

AppDownload* DownloaderPrivate::getApplication(QUrl url)
{
    Q_Q(Downloader);
    QString path = buildDownloadPath();
    AppDownload* appDown = new AppDownload(path, url, _nam);
    q->connect(appDown, SIGNAL(stateChanged()),
        q, SLOT(onDownloadStateChanged()));
    return appDown;
}

AppDownload* DownloaderPrivate::getApplication(QUrl url, QString hash, QCryptographicHash::Algorithm algo)
{
    Q_Q(Downloader);
    QString path = buildDownloadPath();
    AppDownload* appDown = new AppDownload(path, url, hash, algo, _nam);
    q->connect(appDown, SIGNAL(stateChanged()),
        q, SLOT(onDownloadStateChanged()));
    return appDown;
}

void DownloaderPrivate::updateCurrentDownload()
{
    qDebug() << "Updating CURRENT DOWNLOAD.";
    if (_current != NULL)
    {
        AppDownload::State state = _current->state();
        if (state == AppDownload::CANCELED || state == AppDownload::FINISHED)
        {
            qDebug() << "Current was CANCELED or FINISHED";
            // clean resources
            _conn.unregisterObject(_current->path());
            _downloads.removeOne(_current);
            _adaptors.remove(_current->path());
            _current = NULL;

        }
        else
        {
            qDebug() << "Current was not deleted or canceled but paused.";
            _current = NULL;
            return;
        }
    }

    qDebug() << "Updating current download.";
    // loop via the downloads and choose the first that is started or resumed
    for(int index = 0; index < _downloads.count(); index++)
    {
        AppDownload* appDownload = _downloads[index];
        AppDownload::State state = appDownload->state();
        if(state == AppDownload::STARTED || state == AppDownload::RESUMED)
        {
            qDebug() << "Found downlad ready for " << appDownload->url();
            // found the first instance that can be downloaded set it to be first
            _downloads.removeAt(index);
            _downloads.prepend(appDownload);
            _current = appDownload;

            if (state == AppDownload::STARTED)
            {
                qDebug() << "Starting download for" << appDownload->url();
                appDownload->startDownload();
            }
            else
            {
                qDebug() << "Resuming download for " << appDownload->url();
                appDownload->resumeDownload();
            }

            break;
        } // is started or resumed
    } // for
}

void DownloaderPrivate::onDownloadStateChanged()
{
    Q_Q(Downloader);
    // get the appdownload that emited the signal and decide what to do with it
    AppDownload* sender = qobject_cast<AppDownload*>(q->sender());
    switch(sender->state())
    {
        case AppDownload::STARTED:
            // only start the download in the update method
            if (_current == NULL)
                updateCurrentDownload();
            break;
        case AppDownload::PAUSED:
            sender->pauseDownload();
            if (_current->path() == sender->path())
                updateCurrentDownload();
            break;
        case AppDownload::RESUMED:
            // only resume the download in the update method
            updateCurrentDownload();
            break;
        case AppDownload::CANCELED:
            // cancel and remove the download
            sender->cancelDownload();
            if (_current->path() == sender->path())
                updateCurrentDownload();
            break;
        case AppDownload::FINISHED:
            // remove the registered object in dbus, remove the download and the adapter from the list
            if (_current->path() == sender->path())
                updateCurrentDownload();
            break;
        default:
            // do nothing
            break;
    }
}

QDBusObjectPath DownloaderPrivate::createDownload(const QString &url)
{
    Q_Q(Downloader);
    qDebug() << "Creating AppDownload object for " << url;
    AppDownload* appDownload = getApplication(url);
    ApplicationDownloadAdaptor* adaptor = new ApplicationDownloadAdaptor(appDownload);

    // we need to store the ref of both objects, else the mem management will delete them
    _downloads.append(appDownload);
    _adaptors[appDownload->path()] = adaptor;
    bool ret = _conn.registerObject(appDownload->path(), appDownload);
    qDebug() << "New DBus object registered to " << appDownload->path() << ret;

    // emit that the download was created. Usefull in case other processes are interested in them
    QDBusObjectPath objectPath(appDownload->path());
    emit q->downloadCreated(objectPath);
    return objectPath;
}

QDBusObjectPath DownloaderPrivate::createDownloadWithHash(const QString &url, const QString &hash, QCryptographicHash::Algorithm algo)
{
    Q_Q(Downloader);
    qDebug() << "Creating AppDownload object for " << url << "hash" << hash << "algo" << algo;
    AppDownload* appDownload = getApplication(url, hash, algo);
    ApplicationDownloadAdaptor* adaptor = new ApplicationDownloadAdaptor(appDownload);

    // we need to store the ref of both objects, else the mem management will delete them
    _downloads.append(appDownload);
    _adaptors[appDownload->path()] = adaptor;
    bool ret = _conn.registerObject(appDownload->path(), appDownload);
    qDebug() << "New DBus object registered to " << appDownload->path() << ret;

    // emit that the download was created. Usefull in case other processes are interested in them
    QDBusObjectPath objectPath(appDownload->path());
    emit q->downloadCreated(objectPath);
    return objectPath;
}

QList<QDBusObjectPath> DownloaderPrivate::getAllDownloads()
{
    Q_Q(Downloader);
    qDebug() << "Getting all object paths.";

    QList<QDBusObjectPath> paths;
    foreach(AppDownload* appDownload, _downloads)
        paths << QDBusObjectPath(appDownload->path());

    // emit the signal in case any other process is interested
    emit q->downloads(paths);
    return paths;
}

/**
 * PUBLIC IMPLEMENTATION
 */

Downloader::Downloader(QDBusConnection connection, QObject *parent) :
    QObject(parent),
    d_ptr(new DownloaderPrivate(connection, this))
{
}

QDBusObjectPath Downloader::createDownload(const QString &url)
{
    Q_D(Downloader);
    return d->createDownload(url);
}

QDBusObjectPath Downloader::createDownloadMd4(const QString &url, const QString &hash)
{
    Q_D(Downloader);
    return d->createDownloadWithHash(url, hash, QCryptographicHash::Md4);
}

QDBusObjectPath Downloader::createDownloadMd5(const QString &url, const QString &hash)
{
    Q_D(Downloader);
    return d->createDownloadWithHash(url, hash, QCryptographicHash::Md5);
}

QDBusObjectPath Downloader::createDownloadSha1(const QString &url, const QString &hash)
{
    Q_D(Downloader);
    return d->createDownloadWithHash(url, hash, QCryptographicHash::Sha1);
}

QDBusObjectPath Downloader::createDownloadSha224(const QString &url, const QString &hash)
{
    Q_D(Downloader);
    return d->createDownloadWithHash(url, hash, QCryptographicHash::Sha224);
}

QDBusObjectPath Downloader::createDownloadSha256(const QString &url, const QString &hash)
{
    Q_D(Downloader);
    return d->createDownloadWithHash(url, hash, QCryptographicHash::Sha256);
}

QDBusObjectPath Downloader::createDownloadSha384(const QString &url, const QString &hash)
{
    Q_D(Downloader);
    return d->createDownloadWithHash(url, hash, QCryptographicHash::Sha384);
}

QDBusObjectPath Downloader::createDownloadSha512(const QString &url, const QString &hash)
{
    Q_D(Downloader);
    return d->createDownloadWithHash(url, hash, QCryptographicHash::Sha512);
}

QList<QDBusObjectPath> Downloader::getAllDownloads()
{
    Q_D(Downloader);
    return d->getAllDownloads();
}

#include "moc_downloader.cpp"

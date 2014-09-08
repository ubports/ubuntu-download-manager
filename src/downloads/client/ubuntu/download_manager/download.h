/*
 * Copyright 2013-2014 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef UBUNTU_DOWNLOADMANAGER_CLIENT_DOWNLOAD_H
#define UBUNTU_DOWNLOADMANAGER_CLIENT_DOWNLOAD_H

#include <QObject>
#include <QVariantMap>
#include <QString>

namespace Ubuntu {

namespace DownloadManager {

class Error;

/*!
    \class Download
    \brief The Download class allows to control a download that
           was created in the download manager.
    \since 0.3

    The Download class represents a download request that was
    created to  be performed by the download manager.

    Due to the asynchronous nature of the API a download request
    is not performed on the Download object creation but after
    the client has requested it via the \ref Download::start method.

    \note The ownership of the Download is relayed to the caller and
    therefore the client must call delete or deleteLater whenever it
    considers to be appropriate.
*/
class Download : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString ClickPackage READ clickPackage NOTIFY clickPackagedChanged)
    Q_PROPERTY(bool ShowInIndicator READ showInIndicator NOTIFY showIndicatorChanged)
    Q_PROPERTY(QString Title READ title NOTIFY titleChanged)

 public:
    explicit Download(QObject* parent = 0)
        : QObject(parent) {}

    /*!
        \fn void Download::start()

        Notifies the download manager that the download that is
        represented by this download object is ready to be downloaded.

        The normal use case is that the client of the API connects to
        all the download signals that he is interested in and the calls the
        start methods. This pattern ensures that if a download is performed
        that all signals are connected else the client could see cases in
        where no signals are received because the download finished before
        he connected to the signals.
    */
    virtual void start() = 0;

    /*!
        \fn void Download::pause()

        Notifies the download manager that the download represented by this
        download object must be paused.
    */
    virtual void pause() = 0;

    /*!
        \fn void Download::resume()

        Notifies the download manager that the download represented by this
        download object must be resumed. There is no guarantee that nor errors
        will be raised if a not paused download is resumed.
    */
    virtual void resume() = 0;

    /*!
        \fn void Download::cancel()

        Notifies the download manager that the download represented by this
        download object must be canceled and all it resources must be cleaned.
    */
    virtual void cancel() = 0;

    /*!
        \fn void Download::allowMobileDownload(bool allowed)

        Notifies the download manager that the download represented by this
        download object is allowed or not to use the phones mobile data whenever
        the devices is connected to it. If the download is not allows to use
        mobile data it will be automatically paused until a valid network
        connection is present.
    */
    virtual void allowMobileDownload(bool allowed) = 0;

    /*!
        \fn bool isMobileDownloadAllowed()

        Returns if the download represented by this download object is allowed to
        use the phones mobile data connection.
    */
    virtual bool isMobileDownloadAllowed() = 0;

    /*!
        \fn void setDestinationDir(const QString& path);

        Notifies the download manager that the local path of the download
        must be a different one. This call can only be performed if the
        download was not started else it will result in an error.
    */
    virtual void setDestinationDir(const QString& path) = 0;

    /*!
        \fn void setHeaders(QMap<QString, QString> headers)
        \since 0.4

        Allows to set the headers to be used for the download request.
        If the download has been started the operation will result in
        an error.
    */
    virtual void setHeaders(QMap<QString, QString> headers) = 0;

    /*!
        \fn void setMetadata(QVariantMap map)
        \since 0.9

        Allows to set the metadata to be used for the download request.
    */
    virtual void setMetadata(QVariantMap map) = 0;

    /*!
        \fn QMap<QString, QString> headers()
        \since 0.4

        Returns the headers that have been set to be fwd in the download
        request.
    */
    virtual QMap<QString, QString> headers() = 0;

    /*!
        \fn void setThrottle(qulonglong speed)

        Notifies the download manager that the download represented by this
        download object has its download bandwidth limited to the value
        provided by \a speed represented in bytes.
    */
    virtual void setThrottle(qulonglong speed) = 0;

    /*!
        \fn qulonglong throttle()

        Returns the bandwidth limit that was set for the download represented
        by this object. If the result is 0 is that no limit was set.
    */
    virtual qulonglong throttle() = 0;

    /*!
        \fn QString id() const

        Returns the unique identifier that represents the download within
        the download manager.
    */
    virtual QString id() const = 0;

    /*!
        \fn QVariantMap metadata()

        Returns the metadata used upon creation of the download represented
        by the object.
    */
    virtual QVariantMap metadata() = 0;

    /*!
        \fn qulonglong progress()

        Returns the size of all the data downloaded so far in bytes.
    */
    virtual qulonglong progress() = 0;

    /*!
        \fn qulonglong totalSize()

        Returns the totals size estimated for the download. The result can
        be 0 when the download manager was not able to deduce the download
        size due to a wrong configuration of the server.
    */
    virtual qulonglong totalSize() = 0;

    /*!
        \fn bool isError() const

        Returns if the download represented by the object has had an error.
    */
    virtual bool isError() const = 0;

    /*!
        \fn Error* error() const

        Returns the last error encountered by the download object.
    */
    virtual Error* error() const = 0;

    /*!
        \fn QString clickPackage()

        Returns the value of the click package property of the download.
        The property shows the id of a download that represents a new click
        application being downloaded. Only unconfined applications are allowed
        to set this property.
    */
    virtual QString clickPackage() const = 0;

    /*!
        \fn bool showInIndicator()

        Returns the value of the show in indicator property of the download.
        The property states if the download will be shown in the systems transfer
        indicator. Confined applications are allowed to set this property to false
        via the metadata to hide their downloads from being shown in the indicator.
    */
    virtual bool showInIndicator() const = 0;

    /*!
        \fn QString title()

        Returns the value of the title property of the download. The title of the
        download is the string that will be shown in the transfer indicator while
        the download is in progress. Confined applications are allowed to set the
        propertys value via the metadata.
    */
    virtual QString title() const = 0;

 signals:

    /*!
        \fn void Download::canceled(bool success)

        This signal is emitted whenever a download cancellation was requested
        and notifies if the cancellation was successful or not via \a success.
    */
    void canceled(bool success);

    /*!
        \fn void Download::error(Error* error)

        This signal is emitted whenever and error occurred during a download.
        To get more information about the cause of the error the client can check
        the type of the error via \ref Error::type() and cast \a error to the
        appropriate subclass.
    */
    void error(Error* error);

    /*!
        \fn void Download::finished(const QString& path)

        This signal is emitted whenever a download has successfully completed.
        \a path is the absolute local file path where the downloaded file can
        be found.
    */
    void finished(const QString& path);

    /*!
        \fn void Download::paused(bool success)

        This signal is emitted whenever a download paused was requested and
        notifies if the pause was successful or not via \a success.
    */
    void paused(bool success);

    /*!
        \fn void Download::processing(const QString& path)

        This signal is emitted whenever a post processing action is being
        performed on the download file indicated by \a path.
    */
    void processing(const QString &path);

    /*!
        \fn void Download::progress(qulonglong received, qulonglong total)

        This signal is emitted to indicate the progress of the download  where
        \a received is the number of bytes received and \a total is the total
        size of the download.

        \note If the download manager is not able to estimate the size of
        a download, which is something that can happen when the server does
        not correctly send the size back, \a received and \a total will have
        the same value. The fact that the values are the same \tt DOES NOT
        mean that a download was completed for that the signals
        \ref Download::finished(const QString& path) should be used.
    */
    void progress(qulonglong received, qulonglong total);

    /*!
        \fn void Download::resumed(bool success)

        This signal is emitted whenever a download resume was requested and
        notifies if the resume was successful via \a success.
    */
    void resumed(bool success);

    /*!
        \fn void Download::started(bool success)

        This signal is emitted whenever a download start was requested and
        notifies if the start action was successful via \a success.
    */
    void started(bool success);

    /*!
        \fn void Download::clickPackagedChanged(const QString& click)

        This signal is emitted whenever the click property of a download has
        been updated.
    */
    void clickPackagedChanged(const QString& click);

    /*!
        \fn void Download::showIndicatorChanged(bool shown)

        This signal is emitted whenever the show in indicator property of the
        download has been updated.
    */
    void showIndicatorChanged(bool shown);

    /*!
        \fn void Download::titleChanged(const QString& title)

        This signal is emitted whenever the title property of the download
        has been updated.
    */
    void titleChanged(const QString& title);

};

}  // Ubuntu

}  // DownloadManager

#endif  // UBUNTU_DOWNLOADMANAGER_CLIENT_DOWNLOAD_H

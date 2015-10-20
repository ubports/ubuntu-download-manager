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

#ifndef UBUNTU_DOWNLOADMANAGER_CLIENT_MANAGER_H
#define UBUNTU_DOWNLOADMANAGER_CLIENT_MANAGER_H

#include <functional>
#include <QObject>
#include <ubuntu/download_manager/metatypes.h>
#include <ubuntu/download_manager/download_struct.h>
#include <ubuntu/download_manager/group_download_struct.h>


namespace Ubuntu {

namespace DownloadManager {

class Download;
class DownloadsList;
class Error;
class GroupDownload;

/*!
    Callback to be executed that takes a download object created by
    the manager.
*/
typedef std::function<void(Download*)> DownloadCb;

/*!
    Callback to be executed that takes a download list object created by
    the manager.
*/
typedef std::function<void(DownloadsList*)> DownloadsListCb;

/*!
    Callback to be executed that takes a download list object created by
    the manager for a given metadata value.
*/
typedef std::function<void(const QString&, const QString&, DownloadsList*)> MetadataDownloadsListCb;

/*!
    Callback to be executed that takes a group download object created by
    the manager.
*/
typedef std::function<void(GroupDownload*)> GroupCb;

/*!
    \class Manager
    \brief The Manager class is the entry point of the download manager
           API and allows the client to create download requests in the
           download manager.
    \since 0.3

    The Manager is the entry point of the API and allows to create new
    downloads that will be performed by the download manager. The
    class allows to have two different types of managers:

      - Session manager: Connects to the session dbus service.
      - System manager: Connects to the system dbus service.

    The general rule of thumb is that a normal client application must
    connect to the session which contains all the downloads created for
    the user in the current session.
*/
class Manager : public QObject {
    Q_OBJECT

 public:
    explicit Manager(QObject* parent = 0)
        : QObject(parent) {}

    /*!
        \fn void getDownloadForId(const QString& id);

        Returns a download object for the download with the given id. The
        id most be valid else the returned download will be in an unknown
        state meaning that most of the download operations will fail.
    */
    virtual Download* getDownloadForId(const QString& id) = 0;

    /*!
        \fn void createDownload(DownloadStruct downStruct)

        Creates a new download using the data found in the structure.
        The downloadCreated(Download* down) can be used to get
        a pointer to the new created download.

        \note The download object ownership from
              downloadCreated(Download* down) is of the caller and
              the download manager will not delete it.
    */
    virtual void createDownload(DownloadStruct downStruct) = 0;

    /*!
        \fn void createDownload(DownloadStruct downStruct, DownloadCb cb, DownloadCb errCb)

        Creates a new download using the data found in the structure.
        \a cb will be executed when a successful download creation occurs while
        \a errCb will be executed when there was an error during the creation
        of the download object.

        \note Even when the callbacks are executed the
              downloadCreated(Download* down) is emitted.

        \note The download object ownership from
              downloadCreated(Download* down) is of the caller and
              the download manager will not delete it.
    */
    virtual void createDownload(DownloadStruct downStruct,
                                DownloadCb cb,
                                DownloadCb errCb) = 0;

    /*!
        \fn void createDownload(StructList downs, const QString& algorithm, bool allowed3G, const QVariantMap& metadata, StringMap headers)

        Creates a group download with the data found in the structure.
        The groupCreated(GroupDownload* down) signal can be used to
        get a pointer to the new created group download.

        \note The download object ownership from
              downloadCreated(Download* down) is of the caller and
              the download manager will not delete it.
    */
    virtual void createDownload(StructList downs,
                                const QString& algorithm,
                                bool allowed3G,
                                const QVariantMap& metadata,
                                StringMap headers) = 0;

    /*!
        \fn void createDownload(StructList downs, const QString& algorithm, bool allowed3G, const QVariantMap& metadata, StringMap headers, GroupCb cb, GroupCb errCb)

        Creates a group download with the data found in the structure.
        \a cb will be executed when a successful group download creation
        occurs while \a errCb will be executed when there was an error
        during the creation of the group download object.

        \note Even when the callbacks are executed the
              downloadCreated(Download* down) is emitted.

        \note The download object ownership from
              downloadCreated(Download* down) is of the caller and
              the download manager will not delete it.
    */
    virtual void createDownload(StructList downs,
                                const QString& algorithm,
                                bool allowed3G,
                                const QVariantMap& metadata,
                                StringMap headers,
                                GroupCb cb,
                                GroupCb errCb) = 0;
    /*!
        \fn void getAllDownloads()

        Returns all the downloads in the download manager that can be accessed
        by the calling client. If the client is not confined all downloads are
        returned, on the other hand if the client is confined the result will 
        be only those downloads created by the client. The result of the method
        is returned via the downloadsFound signal.
    */
    virtual void getAllDownloads() = 0;

    /*!
        \fn void getAllDownloads(DownloadsListCb cb, DownloadsListCb errCb)

        Returns all the downloads in the download manager that can be accessed
        by the calling client. If the client is not confined all downloads are
        returned, on the other hand if the client is confined the result will 
        be only those downloads created by the client. If the method is a
        success the \a cb is executed else \a errCb is executed.
    */
    virtual void getAllDownloads(DownloadsListCb cb,
                                 DownloadsListCb errCb) = 0;
    /*!
        \fn void getAllDownloadsWithMetadata(const QString &name, const QString &value)

        Returns all the downloads in the download manager that can be accessed
        by the calling client. If the client is not confined all downloads are
        returned, on the other hand if the client is confined the result will 
        be only those downloads created by the client. The result of the method
        is returned via the downloadsWithMetadataFound signal.

    */
    virtual void getAllDownloadsWithMetadata(const QString &name,
                                             const QString &value) = 0;
    /*!
        \fn void getAllDownloadsWithMetadata(const QString &name,
                                             const QString &value,
                                             MetadataDownloadsListCb cb,
                                             MetadataDownloadsListCb errCb)

        Returns all the downloads in the download manager that can be accessed
        by the calling client. If the client is not confined all downloads are
        returned, on the other hand if the client is confined the result will 
        be only those downloads created by the client. If the method is a
        success the \a cb is executed else \a errCb is executed.
    */
    virtual void getAllDownloadsWithMetadata(const QString &name,
                                             const QString &value,
                                             MetadataDownloadsListCb cb,
                                             MetadataDownloadsListCb errCb) = 0;

    /*!
        \fn void getUncollectedDownloads(const QString &appId)

        Returns uncollected downloads in the download manager that can be accessed
        by the calling client. If the client is not confined then the downloads
        returned will be those belonging to the app specified via the appId 
        parameter, on the other hand if the client is confined the result will 
        be only those downloads created by the client. The result of the method
        is returned via the downloadsFound signal.
    */
    virtual void getUncollectedDownloads(const QString &appId) = 0;

    /*!
        \fn void getUncollectedDownloads(const QString &appId)

        Returns uncollected downloads in the download manager that can be accessed
        by the calling client. If the client is not confined then the downloads
        returned will be those belonging to the app specified via the appId 
        parameter, on the other hand if the client is confined the result will 
        be only those downloads created by the client. . If the method is a
        success the \a cb is executed else \a errCb is executed.
    */

    virtual void getUncollectedDownloads(const QString &appId,
                                         DownloadsListCb cb,
                                         DownloadsListCb errCb) = 0;

    /*!
        \fn bool isError() const
        Returns if the manager received an error during the execution
        of a command.
    */
    virtual bool isError() const = 0;

    /*!
        \fn  Error* lastError() const

        Returns the last error that occurred when interacting with the
        download manager.
    */
    virtual Error* lastError() const = 0;

    /*!
        \fn void Download::allowMobileDownload(bool allowed)

        Allows to set a general setting that will ensure that all downloads
        managed by the download manager are allowed to use the phones
        mobile data or not.

        \note Confined applications are not allowed to set this setting
              and therefore an error would happen.
    */
    virtual void allowMobileDataDownload(bool allowed) = 0;

    /*!
        \fn bool isMobileDownloadAllowed()

        Returns if, as a general setting, the downloads created by the
        download manager are allowed to use the phones mobile data.
    */
    virtual bool isMobileDataDownload() = 0;

    /*!
        \fn qulonglong defaultThrottle()

        Returns the default bandwidth limit that was set for the downloads
        that are present in the download manager.
    */
    virtual qulonglong defaultThrottle() = 0;

    /*!
        \fn void setDefaultThrottle(qulonglong speed)

        Allows to set the default bandwidth limit to all the downloads in
        the download manager.

        \note Confined applications are not allowed to set this setting
              and therefore an error would happen.
    */
    virtual void setDefaultThrottle(qulonglong speed) = 0;

    /*!
        \fn void exit()

        Allows to stop the download manager. This method should not be used
        in production because it was added to simplify integration testing
        with the download manager.

        \note Confined applications are not allowed to set this setting
              and therefore an error would happen.
    */
    virtual void exit() = 0;

    /*!
        \fn static Manager* createSessionManager(const QString& path="", QObject* parent=0)

        Creates a new manager that will be connected to the session manager.
        \a path allows to provide the path where the download manager service
        can be found. In the general use case \a path does not need to be
        provided and the default value should be used.

        \note \a path is exposed to allow integration tests with the download
              manager by starting a download manager in a path and later
              providing the path to the client library.
    */
    static Manager* createSessionManager(const QString& path = "", QObject* parent=0);

    /*!
        \fn static Manager* createSystemManager(const QString& path = "", QObject* parent=0)

        Creates a new manager that will be connected to the system manager.
        \a path allows to provide the path where the download manager service
        can be found. In the general use case \a path does not need to be
        provided and the default value should be used.

        \note \a path is exposed to allow integration tests with the download
              manager by starting a download manager in a path and later
              providing the path to the client library.
    */
    static Manager* createSystemManager(const QString& path = "", QObject* parent=0);

 signals:

    /*!
        \fn void downloadCreated(Download* down)

        This signal is emitted whenever a download is created by the
        download manager.
    */
    void downloadCreated(Download* down);
    void downloadsFound(DownloadsList* downloads);
    void downloadsWithMetadataFound(const QString& name,
                                    const QString& value,
                                    DownloadsList* downloads);

    /*!
        \fn void groupCreated(GroupDownload* down)

        This signal is emitted whenever a group download is created by the
        download manager.
    */
    void groupCreated(GroupDownload* down);

};

}  // DownloadManager

}  // Ubuntu

#endif  // UBUNTU_DOWNLOADMANAGER_CLIENT_MANAGER_H

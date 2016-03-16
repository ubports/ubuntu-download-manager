/*
 * Copyright 2014 Canonical Ltd.
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

#include "metadata.h"

namespace Ubuntu {

namespace DownloadManager {

/*!
    \qmltype Metadata
    \instantiates Metadata
    \inqmlmodule Ubuntu.DownloadManager 1.2
    \ingroup download
    \brief Provides additional properties for downloads

    Metadata provides the ability to set a number of special
    properties on a SingleDownload, e.g. to display a download in
    the transfer indicator, or automatically extract a download
    of a zip file upon completion.

    Example usage:

    \qml
    import QtQuick 2.0
    import Ubuntu.Components 1.2
    import Ubuntu.DownloadManager 1.2

    Rectangle {
        width: units.gu(100)
        height: units.gu(20)

        TextField {
            id: text
            placeholderText: "File URL to download..."
            height: 50
            anchors {
                left: parent.left
                right: button.left
                rightMargin: units.gu(2)
            }
        }

        Button {
            id: button
            text: "Download"
            height: 50
            anchors.right: parent.right

            onClicked: {
                single.download(text.text);
            }
        }

        ProgressBar {
            minimumValue: 0
            maximumValue: 100
            value: single.progress
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            SingleDownload {
                id: single
                metadata: Metadata {
                    showInIndicator: true
                    title: "Example download"
                }
            }
        }
    }
    \endqml

    \sa {DownloadManager}
*/


Metadata::Metadata(QObject* parent)
    : QObject(parent) {
}

Metadata::Metadata(QVariantMap map, QObject* parent)
    : QObject(parent),
      _metadata(map) {
}

QString
Metadata::title() const {
    return _metadata.title();
}

void
Metadata::setTitle(QString title) {
    if (title != _metadata.title()) {
        _metadata.setTitle(title);
        emit titleChanged();
    }
}

bool
Metadata::showInIndicator() const {
    return _metadata.showInIndicator();
}

void
Metadata::setShowInIndicator(bool shown) {
    if (shown != _metadata.showInIndicator()) {
        _metadata.setShowInIndicator(shown);
        emit showIndicatorChanged();
    }
}

bool
Metadata::deflate() const {
    return _metadata.deflate();
}

void
Metadata::setDeflate(bool deflate) {
    if (deflate != _metadata.deflate()) {
        _metadata.setDeflate(deflate);
        emit deflateChanged();
    }
}

bool
Metadata::extract() const {
    return _metadata.extract();
}

void
Metadata::setExtract(bool extract) {
    if (extract != _metadata.extract()) {
        _metadata.setExtract(extract);
        emit extractChanged();
    }
}

QVariantMap
Metadata::map() const {
    return QVariantMap(_metadata);
}


/*!
    \qmlproperty bool Metadata::showInIndicator

    Specifies whether this download should be displayed in the transfers
    indicator, defaults to False.
*/

/*!
    \qmlproperty string Metadata::title

    This sets the name to display in the transfer indicator for this
    download, this property is only used if showInIndicator is True.
*/


/*!
    \qmlproperty bool Metadata::extract

    When set to True the download manager will attempt to automatically 
    extract zip files when the download finishes. This property defaults
    to False.
*/

}

}

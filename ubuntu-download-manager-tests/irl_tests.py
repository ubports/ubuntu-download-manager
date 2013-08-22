#!/usr/bin/python
# -*- encoding: utf-8 -*-
#
# Copyright 2013 Canonical Ltd.
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranties of
# MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
# PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# In addition, as a special exception, the copyright holders give
# permission to link the code of portions of this program with the
# OpenSSL library under certain conditions as described in each
# individual source file, and distribute linked combinations
# including the two.
# You must obey the GNU General Public License in all respects
# for all of the code used other than OpenSSL.  If you modify
# file(s) with this exception, you may extend this exception to your
# version of the file(s), but you are not obligated to do so.  If you
# do not wish to do so, delete this exception statement from your
# version.  If you delete this exception statement from all source
# files in the program, then also delete it here.
"""Test the ubuntu downloader."""

import gobject
import dbus
from dbus.mainloop.glib import DBusGMainLoop

DBusGMainLoop(set_as_default=True)

MANAGER_PATH = '/'
MANAGER_IFACE = 'com.canonical.applications.DownloaderManager'
DOWNLOAD_IFACE = 'com.canonical.applications.Download'
IMAGE_FILE = 'http://i.imgur.com/y51njgu.jpg'


def download_created(path):
    """Deal with the download created signal."""
    print 'Download created in %s' % path


def finished_callback(path, loop):
    """Deal with the finis signal."""
    print 'Download performed in "%s"' % path
    loop.quit()


def progress_callback(total, progress):
    """Deal with the progress signals."""
    print 'Progress is %s/%s' % (progress, total)

if __name__ == '__main__':

    bus = dbus.SessionBus()
    loop = gobject.MainLoop()
    manager = bus.get_object('com.canonical.applications.Downloader',
            MANAGER_PATH)
    manager_dev_iface = dbus.Interface(manager, dbus_interface=MANAGER_IFACE)

    # ensure that download created works
    manager_dev_iface.connect_to_signal('downloadCreated', download_created)

    down_path = manager_dev_iface.createDownload(IMAGE_FILE, {}, {})

    download = bus.get_object('com.canonical.applications.Downloader',
            down_path)

    download_dev_iface = dbus.Interface(download, dbus_interface=DOWNLOAD_IFACE)

    # connect to signals
    download_dev_iface.connect_to_signal('progress', progress_callback)
    download_dev_iface.connect_to_signal('finished',
            lambda path: finished_callback(path, loop))

    download_dev_iface.start()

    loop.run()

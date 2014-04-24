/*
 * Copyright 2014 Canonical Ltd.
 *
 * Authors:
 * Manuel de la Pena: manuel.delapena@canonical.com
 *
 * This file is part of ubuntu-download-manager.
 *
 * ubuntu-download-manager is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * ubuntu-download-manager is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Package udm provides a go interface to work with the ubuntu download manager
package udm

import (
	"launchpad.net/go-dbus/v1"
)

// Progress provides how much progress has been performed in a download that was
// already started.
type Progress struct {
	Received uint64
	Total    uint64
}

// internal interface used to simplify testing
type watch interface {
	Cancel() error
	Chanel() chan *dbus.Message
}

// small wrapper used to simplify testing by using the watch interface
type watchWrapper struct {
	watch *dbus.SignalWatch
}

func newWatchWrapper(sw *dbus.SignalWatch) *watchWrapper {
	w := watchWrapper{}
	return &w
}

func (w *watchWrapper) Cancel() error {
	return w.watch.Cancel()
}

func (w *watchWrapper) Chanel() chan *dbus.Message {
	return w.watch.C
}

// interface added to simplify testing
type proxy interface {
	Call(iface, method string, args ...interface{}) (*dbus.Message, error)
}

var readArgs = func(msg *dbus.Message, args ...interface{}) error {
	return msg.Args(args)
}

func connectToSignal(conn *dbus.Connection, path dbus.ObjectPath, signal string) (watch, error) {
	sw, err := conn.WatchSignal(&dbus.MatchRule{
		Type:      dbus.TypeSignal,
		Sender:    DOWNLOAD_SERVICE,
		Interface: DOWNLOAD_INTERFACE,
		Member:    signal,
		Path:      path})
	w := newWatchWrapper(sw)
	return w, err
}

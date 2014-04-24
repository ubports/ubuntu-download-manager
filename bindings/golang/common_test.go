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

package udm

import (
	. "gopkg.in/check.v1"
	"launchpad.net/go-dbus/v1"
	"testing"
)

func Test(t *testing.T) { TestingT(t) }

type fakeProxy struct {
	Interface  string
	MethodName string
	Args       []interface{}
	Err        error
	Result     *dbus.Message
}

func (f *fakeProxy) Call(iface, method string, args ...interface{}) (*dbus.Message, error) {
	// store the called method and return Result
	f.Interface = iface
	f.MethodName = method
	f.Args = args
	if f.Err == nil {
		return f.Result, nil
	}
	return nil, f.Err
}

type FakeWatch struct {
	Canceled bool
	Ch       chan *dbus.Message
}

func newFakeWatch() *FakeWatch {
	ch := make(chan *dbus.Message)
	fw := FakeWatch{false, ch}
	return &fw
}

func (w *FakeWatch) Cancel() error {
	w.Canceled = true
	return nil
}

func (w *FakeWatch) Chanel() chan *dbus.Message {
	return w.Ch
}

// returns a new error that can be used in the tests
func newDBusError() *dbus.Message {
	msg := dbus.NewMethodCallMessage("com.destination", "/path", "com.interface", "method")
	msg.Type = dbus.TypeError
	msg.ErrorName = "com.testing.udm"
	return msg
}

func newDBusReturn() *dbus.Message {
	msg := dbus.NewMethodCallMessage("com.destination", "/path", "com.interface", "method")
	msg.Type = dbus.TypeMethodReturn
	return msg
}

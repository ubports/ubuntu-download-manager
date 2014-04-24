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
	"errors"
	. "gopkg.in/check.v1"
	"launchpad.net/go-dbus/v1"
	"reflect"
)

type UploadSuite struct {
	proxy        *fakeProxy
	upload     *FileUpload
	msg_args     []interface{}
	msg_args_err error
	started_ch   chan bool
	started_w    watch
	canceled_ch  chan bool
	canceled_w   watch
	finished_ch  chan string
	finished_w   watch
	errors_ch    chan error
	errors_w     watch
	progress_ch  chan Progress
	progress_w   watch
}

var _ = Suite(&UploadSuite{})

func (s *UploadSuite) SetUpTest(c *C) {
	s.proxy = &fakeProxy{}
	s.started_ch = make(chan bool)
	s.started_w = newFakeWatch()
	s.canceled_ch = make(chan bool)
	s.canceled_w = newFakeWatch()
	s.finished_ch = make(chan string)
	s.finished_w = newFakeWatch()
	s.errors_ch = make(chan error)
	s.errors_w = newFakeWatch()
	s.progress_ch = make(chan Progress)
	s.progress_w = newFakeWatch()
	s.upload = &FileUpload{nil, s.proxy, "", s.started_ch, s.started_w, s.canceled_ch, s.canceled_w, s.finished_ch, s.finished_w, s.errors_ch, s.errors_w, s.progress_ch, s.progress_w}

	readArgs = func(msg *dbus.Message, args ...interface{}) error {
		for i, arg := range args {
			v := reflect.ValueOf(arg)
			e := v.Elem()
			switch s.msg_args[i].(type) {
			default:
				return errors.New("unexpected type")
			case bool:
				e.SetBool(s.msg_args[i].(bool))
			case uint64:
				e.SetUint(s.msg_args[i].(uint64))
			}
		}
		return s.msg_args_err
	}
}

func (s *UploadSuite) TestProgressError(c *C) {
	s.proxy.Result = newDBusError()
	progress, err := s.upload.Progress()
	c.Assert(s.proxy.Interface, Equals, UPLOAD_INTERFACE)
	c.Assert(s.proxy.MethodName, Equals, "progress")
	c.Assert(progress, Equals, uint64(0))
	c.Assert(err, NotNil)
}

func (s *UploadSuite) TestProgress(c *C) {
	expected_progress := uint64(98)
	s.proxy.Result = newDBusReturn()
	s.msg_args = make([]interface{}, 1)
	s.msg_args[0] = expected_progress
	s.msg_args_err = nil
	progress, err := s.upload.Progress()
	c.Assert(s.proxy.Interface, Equals, UPLOAD_INTERFACE)
	c.Assert(s.proxy.MethodName, Equals, "progress")
	c.Assert(err, IsNil)
	c.Assert(progress, Equals, expected_progress)
}

func (s *UploadSuite) TestMetadataError(c *C) {
	s.proxy.Result = newDBusError()
	metadata, err := s.upload.Metadata()
	c.Assert(s.proxy.Interface, Equals, UPLOAD_INTERFACE)
	c.Assert(s.proxy.MethodName, Equals, "metadata")
	c.Assert(metadata, IsNil)
	c.Assert(err, NotNil)
}

func (s *UploadSuite) TestSetThrotthleError(c *C) {
	throttle := uint64(9)
	s.proxy.Result = newDBusError()
	err := s.upload.SetThrottle(throttle)
	c.Assert(s.proxy.Interface, Equals, UPLOAD_INTERFACE)
	c.Assert(s.proxy.MethodName, Equals, "setThrottle")
	c.Assert(err, NotNil)
	c.Assert(s.proxy.Args[0], Equals, throttle)
}

func (s *UploadSuite) TestSetThrottle(c *C) {
	s.proxy.Result = newDBusReturn()
	err := s.upload.SetThrottle(uint64(9))
	c.Assert(s.proxy.Interface, Equals, UPLOAD_INTERFACE)
	c.Assert(s.proxy.MethodName, Equals, "setThrottle")
	c.Assert(err, IsNil)
}

func (s *UploadSuite) TestThrottle(c *C) {
	expected_throttle := uint64(98)
	s.proxy.Result = newDBusReturn()
	s.msg_args = make([]interface{}, 1)
	s.msg_args[0] = expected_throttle
	s.msg_args_err = nil
	throttle, err := s.upload.Throttle()
	c.Assert(s.proxy.Interface, Equals, UPLOAD_INTERFACE)
	c.Assert(s.proxy.MethodName, Equals, "throttle")
	c.Assert(err, IsNil)
	c.Assert(throttle, Equals, expected_throttle)
}

func (s *UploadSuite) TestAllowMobileUploadError(c *C) {
	s.proxy.Result = newDBusError()
	err := s.upload.AllowMobileUpload(true)
	c.Assert(s.proxy.Interface, Equals, UPLOAD_INTERFACE)
	c.Assert(s.proxy.MethodName, Equals, "allowMobileUpload")
	c.Assert(err, NotNil)
}

func (s *UploadSuite) TestAllowMobileUpload(c *C) {
	expected_allowed := true
	s.proxy.Result = newDBusReturn()
	err := s.upload.AllowMobileUpload(expected_allowed)
	c.Assert(s.proxy.Interface, Equals, UPLOAD_INTERFACE)
	c.Assert(s.proxy.MethodName, Equals, "allowMobileUpload")
	c.Assert(err, IsNil)
	c.Assert(s.proxy.Args[0], Equals, expected_allowed)
}

func (s *UploadSuite) TestIsMobileUploadError(c *C) {
	s.proxy.Result = newDBusError()
	allowed, err := s.upload.IsMobileUpload()
	c.Assert(s.proxy.Interface, Equals, UPLOAD_INTERFACE)
	c.Assert(s.proxy.MethodName, Equals, "isMobileUploadAllowed")
	c.Assert(allowed, Equals, false)
	c.Assert(err, NotNil)
}

func (s *UploadSuite) TestIsMobileUpload(c *C) {
	expected_allowed := true
	s.proxy.Result = newDBusReturn()
	s.msg_args = make([]interface{}, 1)
	s.msg_args[0] = expected_allowed
	s.msg_args_err = nil
	allowed, err := s.upload.IsMobileUpload()
	c.Assert(s.proxy.Interface, Equals, UPLOAD_INTERFACE)
	c.Assert(s.proxy.MethodName, Equals, "isMobileUploadAllowed")
	c.Assert(err, IsNil)
	c.Assert(allowed, Equals, expected_allowed)
}

func (s *UploadSuite) TestStartDBusError(c *C) {
	s.proxy.Result = newDBusError()
	err := s.upload.Start()
	c.Assert(s.proxy.Interface, Equals, UPLOAD_INTERFACE)
	c.Assert(s.proxy.MethodName, Equals, "start")
	c.Assert(err, NotNil)
}

func (s *UploadSuite) TestStartError(c *C) {
	s.proxy.Result = newDBusReturn()
	s.proxy.Err = errors.New("Fake error")
	err := s.upload.Start()
	c.Assert(s.proxy.Interface, Equals, UPLOAD_INTERFACE)
	c.Assert(s.proxy.MethodName, Equals, "start")
	c.Assert(err, NotNil)
}

func (s *UploadSuite) TestCancelDBusError(c *C) {
	s.proxy.Result = newDBusError()
	err := s.upload.Cancel()
	c.Assert(s.proxy.Interface, Equals, UPLOAD_INTERFACE)
	c.Assert(s.proxy.MethodName, Equals, "cancel")
	c.Assert(err, NotNil)
}

func (s *UploadSuite) TestCancelError(c *C) {
	s.proxy.Result = newDBusReturn()
	s.proxy.Err = errors.New("Fake error")
	err := s.upload.Cancel()
	c.Assert(s.proxy.Interface, Equals, UPLOAD_INTERFACE)
	c.Assert(s.proxy.MethodName, Equals, "cancel")
	c.Assert(err, NotNil)
}

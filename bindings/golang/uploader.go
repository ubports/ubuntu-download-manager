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
	"errors"
	"fmt"
	"launchpad.net/go-dbus/v1"
	"runtime"
)

const (
	UPLOAD_SERVICE           = "com.canonical.applications.Uploader"
	UPLOAD_INTERFACE         = "com.canonical.applications.Upload"
	UPLOAD_MANAGER_INTERFACE = "com.canonical.applications.UploadManager"
)

// Upload is the common interface of an upload. It provides all the required
// methods to interact with an upload created by udm.
type Upload interface {
	Progress() (uint64, error)
	Metadata() (map[string]string, error)
	SetThrottle(uint64) error
	Throttle() (uint64, error)
	AllowMobileUpload(bool) error
	IsMobileUpload() (bool, error)
	Start() error
	Cancel() error
	Started() chan bool
	UploadProgress() chan Progress
	Canceled() chan bool
	Finished() chan string
	Error() chan error
}

// FileUpload represents a single file being uploaded by udm.
type FileUpload struct {
	conn       *dbus.Connection
	proxy      proxy
	path       dbus.ObjectPath
	started    chan bool
	started_w  watch
	canceled   chan bool
	canceled_w watch
	finished   chan string
	finished_w watch
	errors     chan error
	error_w    watch
	progress   chan Progress
	progress_w watch
}

func (upload *FileUpload) free() {
	// cancel all watches so that goroutines are done and close the
	// channels
	upload.started_w.Cancel()
	upload.canceled_w.Cancel()
	upload.finished_w.Cancel()
	upload.error_w.Cancel()
	upload.progress_w.Cancel()
}

func cleanUploadData(upload *FileUpload) {
	upload.free()
}

func newFileUpload(conn *dbus.Connection, path dbus.ObjectPath) (*FileUpload, error) {
	proxy := conn.Object(DOWNLOAD_SERVICE, path)
	started_ch := make(chan bool)
	started_w, err := connectToSignal(conn, path, "started")
	if err != nil {
		return nil, err
	}

	canceled_ch := make(chan bool)
	canceled_w, err := connectToSignal(conn, path, "canceled")
	if err != nil {
		return nil, err
	}

	finished_ch := make(chan string)
	finished_w, err := connectToSignal(conn, path, "finished")
	if err != nil {
		return nil, err
	}

	errors_ch := make(chan error)
	errors_w, err := connectToSignal(conn, path, "error")
	if err != nil {
		return nil, err
	}

	progress_ch := make(chan Progress)
	progress_w, err := connectToSignal(conn, path, "progress")
	if err != nil {
		return nil, err
	}

	u := FileUpload{conn, proxy, path, started_ch, started_w, canceled_ch, canceled_w, finished_ch, finished_w, errors_ch, errors_w, progress_ch, progress_w}

	// connect to the diff signals so that we have nice channels that do
	// not expose dbus watchers
	u.connectToStarted()
	u.connectToCanceled()
	u.connectToFinished()
	u.connectToError()
	u.connectToProgress()
	runtime.SetFinalizer(&u, cleanDownloadData)
	return &u, nil
}

// Process returns the process so far in uploading the file.
func (upload *FileUpload) Progress() (progress uint64, err error) {
	reply, err := upload.proxy.Call(UPLOAD_INTERFACE, "progress")
	if err != nil {
		return 0, err
	}

	if reply.Type == dbus.TypeError {
		return 0, fmt.Errorf("DBus Error: %", reply.ErrorName)
	}

	if err = readArgs(reply, &progress); err != nil {
		return 0, err
	}
	return progress, nil
}

// Metadata returns the metadata that was provided at creating time to the upload.
func (upload *FileUpload) Metadata() (metadata map[string]string, err error) {
	reply, err := upload.proxy.Call(UPLOAD_INTERFACE, "metadata")
	if err != nil {
		return nil, err
	}

	if reply.Type == dbus.TypeError {
		return nil, fmt.Errorf("DBus Error: %", reply.ErrorName)
	}

	if err = readArgs(reply, &metadata); err != nil {
		return nil, err
	}
	return metadata, nil
}

// SetThrottle sets the network throttle to be used in the upload.
func (upload *FileUpload) SetThrottle(throttle uint64) (err error) {
	reply, err := upload.proxy.Call(UPLOAD_INTERFACE, "setThrottle", throttle)
	if err != nil {
		return err
	}

	if reply.Type == dbus.TypeError {
		return fmt.Errorf("DBus Error: %", reply.ErrorName)
	}

	return nil
}

// Throttle returns the network throttle that is currently used in the upload.
func (upload *FileUpload) Throttle() (throttle uint64, err error) {
	reply, err := upload.proxy.Call(UPLOAD_INTERFACE, "throttle")
	if err != nil {
		return 0, err
	}

	if reply.Type == dbus.TypeError {
		return 0, fmt.Errorf("DBus Error: %", reply.ErrorName)
	}

	if err = readArgs(reply, &throttle); err != nil {
		return 0, err
	}
	return throttle, nil
}

// AllowMobileUpload returns if the download is allow to use the mobile connect
// connection.
func (upload *FileUpload) AllowMobileUpload(allowed bool) (err error) {
	reply, err := upload.proxy.Call(UPLOAD_INTERFACE, "allowMobileUpload", allowed)
	if err != nil {
		return err
	}

	if reply.Type == dbus.TypeError {
		return fmt.Errorf("DBus Error: %", reply.ErrorName)
	}

	return nil
}

// IsMobileUpload returns if the download will be performed over the mobile data.
func (upload *FileUpload) IsMobileUpload() (allowed bool, err error) {
	reply, err := upload.proxy.Call(UPLOAD_INTERFACE, "isMobileUploadAllowed", allowed)
	if err != nil {
		return false, err
	}

	if reply.Type == dbus.TypeError {
		return false, fmt.Errorf("DBus Error: %", reply.ErrorName)
	}

	if err = readArgs(reply, &allowed); err != nil {
		return false, err
	}
	return allowed, nil
}

func (upload *FileUpload) Start() (err error) {
	reply, err := upload.proxy.Call(UPLOAD_INTERFACE, "start")
	if err != nil {
		return err
	}

	if reply.Type == dbus.TypeError {
		return fmt.Errorf("DBus Error: %", reply.ErrorName)
	}

	return nil
}

// Cancel cancels an upload that was in process and deletes any local files
// that were created.
func (upload *FileUpload) Cancel() (err error) {
	reply, err := upload.proxy.Call(UPLOAD_INTERFACE, "cancel")
	if err != nil {
		return err
	}

	if reply.Type == dbus.TypeError {
		return fmt.Errorf("DBus Error: %", reply.ErrorName)
	}

	return nil
}

func (upload *FileUpload) connectToStarted() {

	go func() {
		for msg := range upload.started_w.Chanel() {
			var started bool
			readArgs(msg, &started)
			upload.started <- started
		}
		close(upload.started)
	}()
}

// Started returns a channel that will be used to communicate the started signals.
func (upload *FileUpload) Started() chan bool {
	return upload.started
}

func (upload *FileUpload) connectToCanceled() {
	go func() {
		for msg := range upload.canceled_w.Chanel() {
			var canceled bool
			readArgs(msg, &canceled)
			upload.canceled <- canceled
		}
		close(upload.canceled)
	}()
}

// Canceled returns a channel that will be used to communicate the canceled signals.
func (upload *FileUpload) Canceled() chan bool {
	return upload.canceled
}

func (upload *FileUpload) connectToFinished() {
	go func() {
		for msg := range upload.finished_w.Chanel() {
			var path string
			readArgs(msg, &path)
			upload.finished <- path
		}
		close(upload.finished)
	}()
}

// Finished returns a channel that will ne used to communicate the finished signals.
func (upload *FileUpload) Finished() chan string {
	return upload.finished
}

func (upload *FileUpload) connectToError() {
	go func() {
		for msg := range upload.error_w.Chanel() {
			var reason string
			readArgs(msg, &reason)
			upload.errors <- errors.New(reason)
		}
		close(upload.errors)
	}()
}

// Error returns the channel that will be used to communicate the error signals.
func (upload *FileUpload) Error() chan error {
	return upload.errors
}

func (upload *FileUpload) connectToProgress() {
	go func() {
		for msg := range upload.progress_w.Chanel() {
			var received uint64
			var total uint64
			readArgs(msg, &received, &total)
			upload.progress <- Progress{received, total}
		}
		close(upload.progress)
	}()
}

// UploadProgress returns a channel that will be used to communicate the progress
// signals.
func (upload *FileUpload) UploadProgress() chan Progress {
	return upload.progress
}

type UploadManager struct {
	conn  *dbus.Connection
	proxy *dbus.ObjectProxy
}

// NewUploadManager creates a new manager that can be used to create download in the
// udm daemon.
func NewUploadManager() (*UploadManager, error) {
	conn, err := dbus.Connect(dbus.SessionBus)
	if err != nil {
		return nil, err
	}

	if err != nil {
		return nil, err
	}

	proxy := conn.Object(UPLOAD_SERVICE, "/")
	d := UploadManager{conn, proxy}
	return &d, nil
}

func (man *UploadManager) CreateUpload(url string, file string, metadata map[string]interface{}, headers map[string]string) (upload Upload, err error) {
	var t map[string]*dbus.Variant
	for key, value := range metadata {
		t[key] = &dbus.Variant{Value: value}
	}
	s := struct {
		U  string
		F  string
		M  map[string]*dbus.Variant
		HD map[string]string
	}{url, file, t, headers}
	var path dbus.ObjectPath
	reply, err := man.proxy.Call(UPLOAD_MANAGER_INTERFACE, "createUpload", s)
	if err != nil || reply.Type == dbus.TypeError {
		return nil, err
	}
	if err = readArgs(reply, &path); err != nil {
		return nil, err
	}
	upload, err = newFileUpload(man.conn, path)
	return upload, err
}

func (man *UploadManager) CreateMmsUpload(url string, file string, hostname string, port int32) (upload Upload, err error) {
	var path dbus.ObjectPath
	reply, err := man.proxy.Call(UPLOAD_MANAGER_INTERFACE, "createMmsUpload", url, file, hostname, port)
	if err != nil || reply.Type == dbus.TypeError {
		return nil, err
	}
	if err = readArgs(reply, &path); err != nil {
		return nil, err
	}
	upload, err = newFileUpload(man.conn, path)
	return upload, err
}

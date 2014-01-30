/*
 * Copyright 2014 Canonical Ltd.
 *
 * Authors:
 * Manuel de la Pena: manuel.delapena@canonical.com
 *
 * This file is part of ubuntu-download-manager.
 *
 * usensord is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * usensord is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

package udm

import (
	"launchpad.net/go-dbus/v1"
	"log"
)

const (
	DOWNLOAD_SERVICE           = "com.canonical.applications.Downloader"
	DOWNLOAD_INTERFACE         = "com.canonical.applications.Download"
	DOWNLOAD_MANAGER_INTERFACE = "com.canonical.applications.DownloadManager"
)

type Progress struct {
	Received uint64
	Total    uint64
}

type Download interface {
	TotalSize() (uint64, error)
	Progress() (uint64, error)
	Metadata() (map[string]string, error)
	SetThrottle(uint64) error
	Throttle() (uint64, error)
	AllowMobileDownload(bool) error
	IsMobileDownload() (bool, error)
	Start() error
	Pause() error
	Resume() error
	Cancel() error
	Started() chan bool
	Paused() chan bool
	DownloadProgress() chan Progress
	Resumed() chan bool
	Canceled() chan bool
	Finished() chan string
	Error() chan string
}

type Manager interface {
	CreateDownload(string, string, string, map[string]interface{}, map[string]string) (Download, error)
	CreateMmsDownload(string, string, int, string, string) (Download, error)
}

type FileDownload struct {
	conn     *dbus.Connection
	proxy    *dbus.ObjectProxy
	path     dbus.ObjectPath
	started  chan bool
	paused   chan bool
	resumed  chan bool
	canceled chan bool
	finished chan string
	errors   chan string
	progress chan Progress
}

func newFileDownload(conn *dbus.Connection, path dbus.ObjectPath) *FileDownload {
	proxy := conn.Object(DOWNLOAD_SERVICE, path)
	started_ch := make(chan bool)
	paused_ch := make(chan bool)
	resumed_ch := make(chan bool)
	canceled_ch := make(chan bool)
	finished_ch := make(chan string)
	errors_ch := make(chan string)
	progress_ch := make(chan Progress)
	d := FileDownload{conn, proxy, path, started_ch, paused_ch, resumed_ch, canceled_ch, finished_ch, errors_ch, progress_ch}
	// connect to the diff signals so that we have nice channels that do
	// not expose dbus watchers
	d.connectToStarted()
	d.connectToPaused()
	d.connectToResumed()
	d.connectToCanceled()
	d.connectToFinished()
	d.connectToError()
	d.connectToProgress()
	return &d
}

func (down *FileDownload) TotalSize() (size uint64, err error) {
	reply, err := down.proxy.Call(DOWNLOAD_INTERFACE, "totalSize")
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: TotalSize")
		return 0, err
	}
	if err = reply.Args(&size); err != nil {
		log.Println("FAILED: Casting size")
		return 0, err
	}
	return size, nil
}

func (down *FileDownload) Progress() (progress uint64, err error) {
	reply, err := down.proxy.Call(DOWNLOAD_INTERFACE, "progress")
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: Progress")
		return 0, err
	}
	if err = reply.Args(&progress); err != nil {
		log.Println("FAILED: Casting progress")
		return 0, err
	}
	return progress, nil
}

func (down *FileDownload) Metadata() (metadata map[string]string, err error) {
	reply, err := down.proxy.Call(DOWNLOAD_INTERFACE, "metadata")
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: Metadata")
		return nil, err
	}
	if err = reply.Args(&metadata); err != nil {
		log.Println("FAILED: Casting metadata")
		return nil, err
	}
	return metadata, nil
}

func (down *FileDownload) SetThrottle(throttle uint64) (err error) {
	reply, err := down.proxy.Call(DOWNLOAD_INTERFACE, "setThrottle", throttle)
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: SetThrottle")
		return err
	}
	return nil
}

func (down *FileDownload) Throttle() (throttle uint64, err error) {
	reply, err := down.proxy.Call(DOWNLOAD_INTERFACE, "throttle")
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: Throttle")
		return 0, err
	}
	if err = reply.Args(&throttle); err != nil {
		log.Println("FAILED: Casting metadata")
		return 0, err
	}
	return throttle, nil
}

func (down *FileDownload) AllowMobileDownload(allowed bool) (err error) {
	reply, err := down.proxy.Call(DOWNLOAD_INTERFACE, "allowGSMDownload", allowed)
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: AllowMobileDownload ")
		return err
	}
	return nil
}

func (down *FileDownload) IsMobileDownload() (allowed bool, err error) {
	reply, err := down.proxy.Call(DOWNLOAD_INTERFACE, "isGSMDownloadAllowed", allowed)
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: IsMobileDownload ")
		return false, err
	}
	if err = reply.Args(&allowed); err != nil {
		log.Println("FAILED: Casting metadata")
		return false, err
	}
	return allowed, nil
}

func (down *FileDownload) Start() (err error) {
	reply, err := down.proxy.Call(DOWNLOAD_INTERFACE, "start")
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: start")
		return err
	}
	return nil
}

func (down *FileDownload) Pause() (err error) {
	reply, err := down.proxy.Call(DOWNLOAD_INTERFACE, "pause")
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: start")
		return err
	}
	return nil
}

func (down *FileDownload) Resume() (err error) {
	reply, err := down.proxy.Call(DOWNLOAD_INTERFACE, "resume")
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: start")
		return err
	}
	return nil
}

func (down *FileDownload) Cancel() (err error) {
	reply, err := down.proxy.Call(DOWNLOAD_INTERFACE, "cancel")
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: start")
		return err
	}
	return nil
}

func (down *FileDownload) connectToStarted() {
	started_w, err := down.conn.WatchSignal(&dbus.MatchRule{
		Type:      dbus.TypeSignal,
		Sender:    DOWNLOAD_SERVICE,
		Interface: DOWNLOAD_INTERFACE,
		Member:    "started",
		Path:      down.path})

	if err != nil {
		log.Println("Could not connect to Started signal")
	}

	go func() {
		for msg := range started_w.C {
			var started bool
			msg.Args(&started)
			down.started <- started
		}
	}()

}

func (down *FileDownload) Started() chan bool {
	return down.started
}

func (down *FileDownload) connectToPaused() {
	paused_w, err := down.conn.WatchSignal(&dbus.MatchRule{
		Type:      dbus.TypeSignal,
		Sender:    DOWNLOAD_SERVICE,
		Interface: DOWNLOAD_INTERFACE,
		Member:    "paused",
		Path:      down.path})

	if err != nil {
		log.Println("Could not connect to Paused signal")
	}

	go func() {
		for msg := range paused_w.C {
			var paused bool
			msg.Args(&paused)
			down.paused <- paused
		}
	}()

}

func (down *FileDownload) Paused() chan bool {
	return down.paused
}

func (down *FileDownload) connectToProgress() {
	paused_w, err := down.conn.WatchSignal(&dbus.MatchRule{
		Type:      dbus.TypeSignal,
		Sender:    DOWNLOAD_SERVICE,
		Interface: DOWNLOAD_INTERFACE,
		Member:    "progress",
		Path:      down.path})

	if err != nil {
		log.Println("Could not connect to Paused signal")
	}

	go func() {
		for msg := range paused_w.C {
			var received uint64
			var total uint64
			msg.Args(&received, &total)
			down.progress <- Progress{received, total}
		}
	}()
}

func (down *FileDownload) DownloadProgress() chan Progress {
	return down.progress
}

func (down *FileDownload) connectToResumed() {
	resumed_w, err := down.conn.WatchSignal(&dbus.MatchRule{
		Type:      dbus.TypeSignal,
		Sender:    DOWNLOAD_SERVICE,
		Interface: DOWNLOAD_INTERFACE,
		Member:    "resumed",
		Path:      down.path})

	if err != nil {
		log.Println("Could not connect to Resumed signal")
	}

	go func() {
		for msg := range resumed_w.C {
			var resumed bool
			msg.Args(&resumed)
			down.resumed <- resumed
		}
	}()

}

func (down *FileDownload) Resumed() chan bool {
	return down.resumed
}

func (down *FileDownload) connectToCanceled() {
	canceled_w, err := down.conn.WatchSignal(&dbus.MatchRule{
		Type:      dbus.TypeSignal,
		Sender:    DOWNLOAD_SERVICE,
		Interface: DOWNLOAD_INTERFACE,
		Member:    "canceled",
		Path:      down.path})

	if err != nil {
		log.Println("Could not connect to Canceled signal")
	}

	go func() {
		for msg := range canceled_w.C {
			var canceled bool
			msg.Args(&canceled)
			down.canceled <- canceled
		}
	}()
}

func (down *FileDownload) Canceled() chan bool {
	return down.canceled
}

func (down *FileDownload) connectToFinished() {
	finished_w, err := down.conn.WatchSignal(&dbus.MatchRule{
		Type:      dbus.TypeSignal,
		Sender:    DOWNLOAD_SERVICE,
		Interface: DOWNLOAD_INTERFACE,
		Member:    "finished",
		Path:      down.path})

	if err != nil {
		log.Println("Could not connect to Finished signal")
	}

	go func() {
		for msg := range finished_w.C {
			var path string
			msg.Args(&path)
			down.finished <- path
		}
	}()
}

func (down *FileDownload) Finished() chan string {
	return down.finished
}

func (down *FileDownload) connectToError() {
	error_w, err := down.conn.WatchSignal(&dbus.MatchRule{
		Type:      dbus.TypeSignal,
		Sender:    DOWNLOAD_SERVICE,
		Interface: DOWNLOAD_INTERFACE,
		Member:    "error",
		Path:      down.path})

	if err != nil {
		log.Println("Could not connect to Error signal")
	}

	go func() {
		for msg := range error_w.C {
			var reason string
			msg.Args(&reason)
			down.errors <- reason
		}
	}()
}

func (down *FileDownload) Error() chan string {
	return down.errors
}

type DownloadManager struct {
	conn  *dbus.Connection
	proxy *dbus.ObjectProxy
}

func NewDownloadManager() (*DownloadManager, error) {
	conn, err := dbus.Connect(dbus.SessionBus)
	if err != nil {
		log.Printf("Connection error:")
	}

	if err != nil {
		log.Printf("Error: %s\n")
	}

	proxy := conn.Object(DOWNLOAD_SERVICE, "/")
	d := DownloadManager{conn, proxy}
	return &d, nil
}

func (man *DownloadManager) CreateDownload(url string, hash string, algo string, metadata map[string]interface{}, headers map[string]string) (down Download, err error) {
	var t map[string]*dbus.Variant
	for key, value := range metadata {
		log.Println("Key:", key, "Value:", value)
		t[key] = &dbus.Variant{Value: value}
	}
	s := struct {
		U  string
		H  string
		A  string
		M  map[string]*dbus.Variant
		HD map[string]string
	}{url, hash, algo, t, headers}
	var path dbus.ObjectPath
	reply, err := man.proxy.Call(DOWNLOAD_MANAGER_INTERFACE, "createDownload", s)
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: createDownload")
		return nil, err
	}
	if err = reply.Args(&path); err != nil {
		log.Println("FAILED: Casting path")
		return nil, err
	}
	down = newFileDownload(man.conn, path)
	return down, nil
}

func (man *DownloadManager) CreateMmsDownload(url string, hostname string, port int, username string, password string) (down Download, err error) {
	var path dbus.ObjectPath
	reply, err := man.proxy.Call(DOWNLOAD_MANAGER_INTERFACE, "createMmsDownload", url, hostname, port, username, password)
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: createMmsDownload")
		return nil, err
	}
	if err = reply.Args(&path); err != nil {
		log.Println("FAILED: Casting path")
		return nil, err
	}
	down = newFileDownload(man.conn, path)
	return down, nil
}

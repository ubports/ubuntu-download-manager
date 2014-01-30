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

// Package udm provides a go interface to work with the ubuntu download manager
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

const (
	MD5 = "md5"
	SHA1 = "sha1"
	SHA224 = "sha224"
	SHA256 = "sha256"
	SHA384 = "sha384"
	SHA512 = "sha512"
)

const (
	LOCAL_PATH = "local-path"
	OBJECT_PATH = "objectpath"
	POST_DOWNLOAD_COMMAND = "post-download-command"
)

// Progress provides how much progress has been performed in a download that was
// already started.
type Progress struct {
	Received uint64
	Total    uint64
}

// Download is the common interface of a download. It provides all the required 
// methods to interact with a download created by udm.
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

// Manager is the single point of entry of the API. Allows to interact with the 
// general setting of udm as well as to create downloads at will.
type Manager interface {
	CreateDownload(string, string, string, map[string]interface{}, map[string]string) (Download, error)
	CreateMmsDownload(string, string, int, string, string) (Download, error)
}

// FileDownload represents a single file being downloaded by udm.
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

func (down *FileDownload) connectToSignal(signal string) (*dbus.SignalWatch, error) {
	w, err := down.conn.WatchSignal(&dbus.MatchRule{
		Type:      dbus.TypeSignal,
		Sender:    DOWNLOAD_SERVICE,
		Interface: DOWNLOAD_INTERFACE,
		Member:    signal,
		Path:      down.path})
	return w, err
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

// TotalSize returns the total size of the file being downloaded.
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

// Process returns the process so far in downloading the file.
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

// Metadata returns the metadata that was provided at creating time to the download.
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

// SetThrottle sets the network throttle to be used in the download.
func (down *FileDownload) SetThrottle(throttle uint64) (err error) {
	reply, err := down.proxy.Call(DOWNLOAD_INTERFACE, "setThrottle", throttle)
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: SetThrottle")
		return err
	}
	return nil
}

// Throttle returns the network throttle that is currently used in the download.
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

// AllowMobileDownload returns if the download is allow to use the mobile connect
// connection.
func (down *FileDownload) AllowMobileDownload(allowed bool) (err error) {
	reply, err := down.proxy.Call(DOWNLOAD_INTERFACE, "allowGSMDownload", allowed)
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: AllowMobileDownload ")
		return err
	}
	return nil
}

// IsMobileDownload returns if the download will be performed over the mobile data.
func (down *FileDownload) IsMobileDownload() (allowed bool, err error) {
	reply, err := down.proxy.Call(DOWNLOAD_INTERFACE, "isGSMDownloadAllowed", allowed)
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: IsMobileDownload")
		return false, err
	}
	if err = reply.Args(&allowed); err != nil {
		log.Println("FAILED: Casting metadata")
		return false, err
	}
	return allowed, nil
}

// Start tells udm that the download is ready to be peformed and that the client is
// ready to recieve signals. The following is a commong pattern to be used when
// creating downloads in udm.
//
//     man, err := udm.NewDownloadManager() 
//     if err != nil {
//     }
//     
//     // variables used to create the download
//
//     url := "http://www.python.org/ftp/python/3.3.3/Python-3.3.3.tar.xz"
//     hash := "8af44d33ea3a1528fc56b3a362924500"
//     hashAlgo := MD5
//     var metadata map[string]interface{}
//     var headers map[string]string
//    
//     // create the download BUT do not start downloading just yet
//     down, err := man.CreateDownload(url, hash, hashAlgo, metadata, headers)
//
//     // connect routines to the download channels so that we can get the 
//     // information of the download the channel will not get any data until the
//     // Start is called.
//    
//     started_signal := down.Started()
//     go func() {
//         <-started_signal
//         fmt.Println("Download started")
//     }()
//     progress_signal := down.DownloadProgress()
//     go func() {
//         for progress := range p {
//             fmt.Printf("Recieved %d out of %d\n", progress.Received, progress.Total)
//         }
//     }()
//
//     finished_signal := down.Finished()
//
//     // start download
//     down.Start()
//    
//     // block until we are finished downloading
//     <- finished_signal
func (down *FileDownload) Start() (err error) {
	reply, err := down.proxy.Call(DOWNLOAD_INTERFACE, "start")
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: start")
		return err
	}
	return nil
}

// Pause pauses a download that was started and if not nothing is done.
func (down *FileDownload) Pause() (err error) {
	reply, err := down.proxy.Call(DOWNLOAD_INTERFACE, "pause")
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: start")
		return err
	}
	return nil
}

// Resumes a download that was paused or does nothing otherwise.
func (down *FileDownload) Resume() (err error) {
	reply, err := down.proxy.Call(DOWNLOAD_INTERFACE, "resume")
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: start")
		return err
	}
	return nil
}

// Cancel cancels a download that was in process and deletes any local files
// that were created.
func (down *FileDownload) Cancel() (err error) {
	reply, err := down.proxy.Call(DOWNLOAD_INTERFACE, "cancel")
	if err != nil || reply.Type == dbus.TypeError {
		log.Println("FAILED: start")
		return err
	}
	return nil
}

func (down *FileDownload) connectToStarted() {
	started_w, err := down.connectToSignal("started")
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

// Started returns a channel that will be used to communicate the started signals.
func (down *FileDownload) Started() chan bool {
	return down.started
}

func (down *FileDownload) connectToPaused() {
	paused_w, err := down.connectToSignal("paused")
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

// Paused returns a channel that will be used to communicate the paused signals.
func (down *FileDownload) Paused() chan bool {
	return down.paused
}

func (down *FileDownload) connectToProgress() {
	paused_w, err := down.connectToSignal("progress")
	if err != nil {
		log.Println("Could not connect to Progress signal")
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

// DownloadProgress returns a channel that will be used to communicate the progress
// signals.
func (down *FileDownload) DownloadProgress() chan Progress {
	return down.progress
}

func (down *FileDownload) connectToResumed() {
	resumed_w, err := down.connectToSignal("resumed")
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

// Resumed returns a channel that will be used to communicate the paused signals.
func (down *FileDownload) Resumed() chan bool {
	return down.resumed
}

func (down *FileDownload) connectToCanceled() {
	canceled_w, err := down.connectToSignal("canceled")
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

// Canceled returns a channel that will be used to communicate the canceled signals.
func (down *FileDownload) Canceled() chan bool {
	return down.canceled
}

func (down *FileDownload) connectToFinished() {
	finished_w, err := down.connectToSignal("finished")
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

// Finished returns a channel that will ne used to communicate the finished signals.
func (down *FileDownload) Finished() chan string {
	return down.finished
}

func (down *FileDownload) connectToError() {
	error_w, err := down.connectToSignal("error")
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

// Error returns the channel that will be used to communicate the error signals.
func (down *FileDownload) Error() chan string {
	return down.errors
}

type DownloadManager struct {
	conn  *dbus.Connection
	proxy *dbus.ObjectProxy
}

// NewDownloadManager creates a new manager that can be used to create download in the
// udm daemon.
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

// CreateDownload creates a new download in the udm daemon that can be used to get
// a remote resource. Udm allows to pass a hash signature and method that will be
// check once the download has been complited.
// 
// The download hash can be one of the the following constants:
//
//   MD5 
//   SHA1
//   SHA224
//   SHA256
//   SHA384
//   SHA512
//
// The metadata attribute can be used to pass extra information to the udm daemon
// that will just be considered if the caller is not a apparmor confined application.
//
//     LOCAL_PATH => allows to provide the local path for the download.
//     OBJECT_PATH => allows to provide the object path to be used in the dbus daemon.
//     POST_DOWNLOAD_COMMAND => allows to provide a command that will be executed on the
//         download 
// 
// The headers attribute allows to provide extra headers to be used in the request used
// to perform the download.
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

// CreateMmsDownload creates an mms download that will be performed right away. An
// mms download only uses mobile that and an apn proxy to download a multime media
// message.
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

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
}

type Manager interface {
	CreateDownload(string, string, string, map[string]interface{}, map[string]string) (Download, error)
	CreateMmsDownload(string, string, int, string, string) (Download, error)
}

type FileDownload struct {
	proxy *dbus.ObjectProxy
}

func newFileDownload(conn *dbus.Connection, path dbus.ObjectPath) *FileDownload {
	proxy := conn.Object(DOWNLOAD_SERVICE, path)
	d := FileDownload{proxy}
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

type DownloadManager struct {
	conn  *dbus.Connection
	proxy *dbus.ObjectProxy
}

func NewDownloadManager() (*DownloadManager, error) {
	conn, err := dbus.Connect(dbus.SessionBus)
	if err != nil {
		log.Fatal("Connection error:", err)
	}

	if err != nil {
		log.Fatal("Error: %s\n", err)
	}

	proxy := conn.Object(DOWNLOAD_SERVICE, "/")
	d := DownloadManager{conn, proxy}
	return &d, nil
}

func (man *DownloadManager) CreateDownload(url string, hash string, algo string, metadata map[string]interface{}, headers map[string]string) (down Download, err error) {
	s := struct {
		u  string
		h  string
		a  string
		m  map[string]interface{}
		hd map[string]string
	}{url, hash, algo, metadata, headers}
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

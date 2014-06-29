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

#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/keywords/channel.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include <ubuntu/download_manager/download_struct.h>
#include <ubuntu/download_manager/logging/logger.h>

typedef boost::log::sources::severity_channel_logger_mt<
    Ubuntu::DownloadManager::Logging::Logger::Level,
    std::string> logger_mt;

namespace Ubuntu {

namespace DownloadManager {

namespace Logging {

class LoggerPrivate {
 public:
    LoggerPrivate() { }

    void init(Logger::Level lvl, const QString& path) {
        if (_lg == nullptr) {
            Q_UNUSED(path);
            _lg = new logger_mt(
                boost::log::keywords::channel = "ubuntu_download_manager",
                boost::log::keywords::severity = lvl);
        } else {
            log(Logger::Critical, "Logger init called more than once.");
        }
    }

    void log(Logger::Level lvl, const QString& msg) {
        if (_lg != nullptr) {
            auto rec = _lg->open_record(boost::log::keywords::severity = lvl);
            if (rec) {
                boost::log::record_ostream strm(rec);
                strm << msg.toStdString();
                strm.flush();
                _lg->push_record(boost::move(rec));
            }
        }
    }

    void log(Logger::Level lvl, const QStringList& msgList) {
        auto msg = msgList.join(" ");
        log(lvl, msg);
    }

    void log(Logger::Level lvl, const QString& msg, QMap<QString, QString> map) {
        auto fmtMsg = msg.arg(toString(map));
        log(lvl, fmtMsg);
    }

    void log(Logger::Level lvl, const QString& msg, DownloadStruct downStruct) {
        QStringList list;

        list << "\n\t\tUrl: " << downStruct.getUrl()
             << "\n\t\tHash Algorithm: '" << downStruct.getAlgorithm() << "'"
             << "\n\t\tMetadata: " << toString(downStruct.getMetadata())
             << "\n\t\tHeaders: " << toString(downStruct.getHeaders());

        auto fmtMsg = msg.arg(list.join(""));
        log(lvl, fmtMsg);
    }

 private:
    QString toString(QMap<QString, QString> map) {
        QStringList list;
        list << "{";
        int index = map.keys().count();
        foreach(const QString& key, map.keys()) {
            list << "'" << key << "': '" << map[key] << "'";
            if (index != 1) {
                list << ", ";
            }
            index--;
        }
        list << "}";
        return list.join("");
    }

    QString toString(QVariantMap map) {
        QStringList list;
        list << "{";
        int index = map.keys().count();
        foreach(const QString& key, map.keys()) {
            list << "'" << key << "': '"
                << map[key].toString() << "'";
    	    if (index != 1) {
                list << ", ";
    	    }
            index--;
        }
        list << "}";
        return list.join("");
    }

 private:
    logger_mt* _lg;
};

LoggerPrivate* Logger::_private = new LoggerPrivate();
QMutex Logger::_mutex;

void
Logger::init(Logger::Level lvl, const QString& path) {
    _mutex.lock();
    _private->init(lvl, path);
    _mutex.unlock();
}

void
Logger::log(Logger::Level lvl, const QString& msg) {
    _private->log(lvl, msg);
}

void
Logger::log(Logger::Level lvl, const QStringList& msg) {
    _private->log(lvl, msg);
}

void
Logger::log(Level lvl, const QString& msg, QMap<QString, QString> map) {
    _private->log(lvl, msg, map);
}

void
Logger::log(Level lvl, const QString& msg, DownloadStruct downStruct) {
    _private->log(lvl, msg, downStruct);
}

}  // logging

}  // Transfers

}  // Ubuntu

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

#include <QCoreApplication>
#include <QTimer>

#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <ubuntu/downloads/extractor/factory.h>
#include <ubuntu/downloads/extractor/deflator.h>

namespace po = boost::program_options;
namespace ude = Ubuntu::DownloadManager::Extractor;

int main(int argc, char *argv[]) {
    // define the options using boost program options

    QCoreApplication a(argc, argv);
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("unzip", "unzip will be used to extract the data.")
        ("untar", "untar will be used to extract the data.")
        ("path", po::value<std::string>(), "The path to be extracted.")
	("destination", po::value<std::string>(), "The path to be extracted.")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    std::string destination;
    std::string path;
    std::string extractor;

    if (vm.count("path")) {
        path = vm["path"].as<std::string>();
    } else {
        return 1;
    }

    if (vm.count("destination")) {
        destination = vm["destination"].as<std::string>();
    } else {
        return 1;
    }


    if (vm.count("unzip") && vm.count("untar")) {
        std::cout << "Cannot use more that one extractor." << std::endl;
        return 1;
    }

    if (vm.count("unzip")) {
        extractor = std::string("unzip");
    }

    if (vm.count("untar")) {
        extractor = std::string("untar");
    }

    auto factory = new ude::Factory();
    auto deflator = factory->deflator(QString::fromStdString(extractor),
        QString::fromStdString(path),
        QString::fromStdString(destination));

    if (factory->isError()) {
        std::cout << factory->lastError().toStdString() << std::endl;
        return 1;
    } else {
	// use a single shot timer to execute the extraction
        QTimer::singleShot(0, deflator, SLOT(deflate()));
    }

    return a.exec();
}

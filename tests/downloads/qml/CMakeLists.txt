# Copyright © 2014 Canonical Ltd.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Authored by: Manuel de la Peña <manuel.delapena@canonical.com>

set(QML_TESTS
	test_single_download
	test_metadata
)

foreach(test ${QML_TESTS})
        # set the sources per test
        set(${test}_QML_SOURCES
                ${test}.cpp
        )

        set(${test}_QML_HEADERS
		download.h
		error.h
		manager.h
		testable_single_download.h
                ${test}.h
        )

endforeach(test)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5DBus_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5Test_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${DBUS_INCLUDE_DIRS})
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${GMOCK_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/common/public)
include_directories(${CMAKE_SOURCE_DIR}/src/downloads/common)
include_directories(${CMAKE_SOURCE_DIR}/src/downloads/client)
include_directories(${CMAKE_SOURCE_DIR}/src/downloads/qml)
include_directories(${CMAKE_SOURCE_DIR}/tests/common)

link_directories(${NIH_DBUS_LIBDIR})
link_directories(${GLOG_DBUS_LIBDIR})

set(QML_TESTS_LIBS
	${NIH_DBUS_LIBRARIES}
	${GLOG_LIBRARIES}
        ${Qt5Core_LIBRARIES}
        ${Qt5Sql_LIBRARIES}
        ${Qt5DBus_LIBRARIES}
	${Qt5Test_LIBRARIES}
        ${GMOCK_LIBRARY}
        ${GTEST_BOTH_LIBRARIES}
	udm-common
	ubuntu-download-manager-common
        ubuntu-download-manager-test-lib
        UbuntuDownloadManager
)

foreach(test ${QML_TESTS})
        # set targets, linked libs and test
        add_executable(${test}_qml
                ${${test}_QML_SOURCES}
                ${${test}_QML_HEADERS}
        )

        target_link_libraries(${test}_qml
                ${QML_TESTS_LIBS}
        )


        ADD_TEST(NAME qml_${test} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${test}_qml)
endforeach(test)


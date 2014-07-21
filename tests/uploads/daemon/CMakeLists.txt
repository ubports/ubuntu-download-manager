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

set(UPLOAD_DAEMON_TESTS
        test_file_upload
        test_mms_upload
        test_upload_factory
)

foreach(test ${UPLOAD_DAEMON_TESTS})
        # set the sources per test
        set(${test}_UPLOAD_DAEMON_SOURCES
                squid.cpp
                ${test}.cpp
        )

        set(${test}_UPLOAD_DAEMON_HEADERS
                squid.h
        )

endforeach(test)


include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5SystemInfo_INCLUDE_DIRS})
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
include_directories(${CMAKE_SOURCE_DIR}/src/common/priv)
include_directories(${CMAKE_SOURCE_DIR}/src/uploads/common)
include_directories(${CMAKE_SOURCE_DIR}/src/uploads/priv)
include_directories(${CMAKE_SOURCE_DIR}/tests/common)

link_directories(${NIH_DBUS_LIBDIR})
link_directories(${GLOG_DBUS_LIBDIR})

set(UPLOAD_DAEMON_TESTS_LIBS
        ${NIH_DBUS_LIBRARIES}
        ${GLOG_LIBRARIES}
        ${Qt5Core_LIBRARIES}
        ${Qt5Sql_LIBRARIES}
        ${Qt5DBus_LIBRARIES}
        ${Qt5Test_LIBRARIES}
        ${GMOCK_LIBRARY}
        ${GTEST_BOTH_LIBRARIES}
        udm-common
        udm-priv-common
        ubuntu-upload-manager-common
        ubuntu-upload-manager-priv
        ubuntu-download-manager-test-lib
)


foreach(test ${UPLOAD_DAEMON_TESTS})
        # set targets, linked libs and test
        add_executable(${test}_daemon
                ${${test}_UPLOAD_DAEMON_SOURCES}
                ${${test}_UPLOAD_DAEMON_HEADERS}
        )

        target_link_libraries(${test}_daemon
                ${UPLOAD_DAEMON_TESTS_LIBS}
        )


ADD_TEST(NAME daemon_${test} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${test}_daemon)
endforeach(test)


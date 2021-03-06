# Copyright © 2014-2015 Canonical Ltd.
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

set(DAEMON_TESTS
        test_apn_request_factory
        test_apparmor
        test_base_download
        test_cancel_download_transition
        test_daemon
        test_download
        test_download_factory
        test_download_manager
        test_downloads_db
        test_file_download_sm
        test_filename_mutex
        test_final_state
        test_group_download
        test_metadata
        test_mms_download
        test_network_error_transition
        test_resume_download_transition
        test_ssl_error_transition
        test_start_download_transition
        test_stop_request_transition
        test_transfers_queue
)

foreach(test ${DAEMON_TESTS})
        # set the sources per test
        set(${test}_DAEMON_SOURCES
                ${test}.cpp
        )

        set(${test}_DAEMON_HEADERS
                application.h
                cryptographic_hash.h
                database.h
                dbus_proxy.h
                dbus_proxy_factory.h
                download.h
                factory.h
                filename_mutex.h
                group_download.h
                manager.h
                pending_reply.h
                queue.h
                sm_file_download.h
                timer.h
                transfer.h
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
include_directories(${CMAKE_SOURCE_DIR}/src/common/priv)
include_directories(${CMAKE_SOURCE_DIR}/src/downloads/common)
include_directories(${CMAKE_SOURCE_DIR}/src/downloads/client)
include_directories(${CMAKE_SOURCE_DIR}/src/downloads/priv)
include_directories(${CMAKE_SOURCE_DIR}/tests/common)

link_directories(${NIH_DBUS_LIBDIR})
link_directories(${GLOG_DBUS_LIBDIR})

set(DAEMON_TESTS_LIBS
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
        ubuntu-download-manager-common
        ubuntu-download-manager-priv
        ubuntu-download-manager-test-lib
)


foreach(test ${DAEMON_TESTS})
        # set targets, linked libs and test
        add_executable(${test}_daemon
                ${${test}_DAEMON_SOURCES}
                ${${test}_DAEMON_HEADERS}
        )

        target_link_libraries(${test}_daemon
                ${DAEMON_TESTS_LIBS}
        )


ADD_TEST(NAME daemon_${test} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${test}_daemon)
endforeach(test)


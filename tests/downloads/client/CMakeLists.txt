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


set(CLIENT_TESTS
        test_client_download
	#        test_download_manager_watch
	#        test_download_watch
	#        test_client_manager
)

foreach(test ${CLIENT_TESTS})
        # set the sources per test
        set(${test}_CLIENT_SOURCES
                ${test}.cpp
        )

        set(${test}_CLIENT_HEADERS
                ${test}.h
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
include_directories(${CMAKE_SOURCE_DIR}/src/downloads/common)
include_directories(${CMAKE_SOURCE_DIR}/src/downloads/client)
include_directories(${CMAKE_SOURCE_DIR}/src/downloads/priv)
include_directories(${CMAKE_SOURCE_DIR}/tests/common)

link_directories(${NIH_DBUS_LIBDIR})
link_directories(${GLOG_DBUS_LIBDIR})

find_program(DBUS_RUNNER dbus-test-runner)

set(CLIENT_TESTS_LIBS
	${NIH_DBUS_LIBRARIES}
	${GLOG_LIBRARIES}
        ${Qt5Network_LIBRARIES}
	${Qt5Test_LIBRARIES}
	udm-common
	udm-priv-common
	ubuntu-download-manager-common
	ubuntu-download-manager-client
	ubuntu-download-manager-priv
        ubuntu-download-manager-test-lib
)

if(DBUS_RUNNER)
        foreach(test ${CLIENT_TESTS})
                # set targets, linked libs and test
                add_executable(${test}_client
                        ${${test}_CLIENT_SOURCES}
                        ${${test}_CLIENT_HEADERS}
                )

                target_link_libraries(${test}_client
                        ${CLIENT_TESTS_LIBS}
                )

                ADD_TEST(NAME client_${test} COMMAND dbus-test-runner -m 180 --task=${CMAKE_CURRENT_BINARY_DIR}/${test}_client -c)

        endforeach(test)
else(DBUS_RUNNER)
        message(WARNING "dbus-test-runner binary not found tests will be disabled")
endif(DBUS_RUNNER)

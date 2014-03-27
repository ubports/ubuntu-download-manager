set(DAEMON_TESTS
	test_apn_request_factory
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
                base_testcase.cpp
                ${test}.cpp
        )

        set(${test}_DAEMON_HEADERS
                application.h
                apparmor.h
                base_testcase.h
                cryptographic_hash.h
		database.h
                dbus_connection.h
		download.h
                factory.h
                file_manager.h
                filename_mutex.h
		manager.h
                matchers.h
		network_reply.h
                process.h
                process_factory.h
                queue.h
                request_factory.h
                sm_file_download.h
                system_network_info.h
                timer.h
                transfer.h
                uuid_factory.h
                ${test}.h
        )

endforeach(test)

set(CLIENT_TESTS
        test_client_download
        test_download_manager_watch
        test_download_watch
        test_client_manager
)

foreach(test ${CLIENT_TESTS})
        # set the sources per test
        set(${test}_CLIENT_SOURCES
                base_testcase.cpp
                daemon_testcase.cpp
                local_tree_testcase.cpp
                testing_interface.cpp
                ${test}.cpp
        )

        set(${test}_CLIENT_HEADERS
                base_testcase.h
                daemon_testcase.h
                local_tree_testcase.h
                testing_interface.h
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

link_directories(${NIH_DBUS_LIBDIR})
link_directories(${GLOG_DBUS_LIBDIR})

if(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
add_custom_target(download-daemon-tests-data ALL
        COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
)
endif(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")

set(DAEMON_TESTS_LIBS
	${NIH_DBUS_LIBRARIES}
	${GLOG_LIBRARIES}
        ${Qt5Core_LIBRARIES}
        ${Qt5Sql_LIBRARIES}
	${Qt5Test_LIBRARIES}
        ${GMOCK_LIBRARY}
        ${GTEST_BOTH_LIBRARIES}
	udm-common
	udm-priv-common
	ubuntu-download-manager-common
	ubuntu-download-manager-priv
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

                ADD_TEST(NAME client_${test} COMMAND dbus-test-runner -m 120 --task=${CMAKE_CURRENT_BINARY_DIR}/${test}_client -c)

        endforeach(test)
else(DBUS_RUNNER)
        message(WARNING "dbus-test-runner binary not found tests will be disabled")
endif(DBUS_RUNNER)

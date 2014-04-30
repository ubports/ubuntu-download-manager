set(TESTS
        test_download
        test_download_manager_watch
        test_download_watch
        test_manager
)

foreach(test ${TESTS})
        # set the sources per test
        set(${test}_SOURCES
                local_tree_testcase.cpp
                ${test}.cpp
        )

        set(${test}_HEADERS
                local_tree_testcase.h
                ${test}.h
        )

endforeach(test)

include_directories(${Qt5SystemInfo_INCLUDE_DIRS})
include_directories(${Qt5DBus_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5Test_INCLUDE_DIRS})
include_directories(${DBUS_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/udm-common)
include_directories(${CMAKE_SOURCE_DIR}/udm-priv-common)
include_directories(${CMAKE_SOURCE_DIR}/ubuntu-download-manager-common)
include_directories(${CMAKE_SOURCE_DIR}/ubuntu-download-manager-client)
include_directories(${CMAKE_SOURCE_DIR}/ubuntu-download-manager-priv)
include_directories(${CMAKE_SOURCE_DIR}/ubuntu-download-manager-test-lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

link_directories(${NIH_DBUS_LIBDIR})
link_directories(${GLOG_DBUS_LIBDIR})

if(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
add_custom_target(client-tests-data ALL
        COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
)
endif(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")

find_program(DBUS_RUNNER dbus-test-runner)

set(LIBS
	${NIH_DBUS_LIBRARIES}
	${GLOG_LIBRARIES}
	${Qt5Test_LIBRARIES}
	udm-common
	udm-priv-common
	ubuntu-download-manager-common
	ubuntu-download-manager-client
	ubuntu-download-manager-priv
	ubuntu-download-manager-test-lib
)

if(DBUS_RUNNER)
        foreach(test ${TESTS})
                # set targets, linked libs and test
                add_executable(${test}
	                ${${test}_SOURCES}
                        ${${test}_HEADERS}
                )

                target_link_libraries(${test}
                        ${LIBS}
                )

                ADD_TEST(NAME ${test} COMMAND dbus-test-runner -m 120 --task=${CMAKE_CURRENT_BINARY_DIR}/${test} -c)

        endforeach(test)
else(DBUS_RUNNER)
        message(WARNING "dbus-test-runner binary not found tests will be disabled")
endif(DBUS_RUNNER)



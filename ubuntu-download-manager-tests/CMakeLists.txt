set(TARGET ubuntu-download-manager-tests)

set(SOURCES
        main.cpp
        downloads/test_base_download.cpp
        downloads/test_daemon.cpp
        downloads/test_download.cpp
        downloads/test_download_factory.cpp
        downloads/test_download_manager.cpp
        downloads/test_downloads_db.cpp
        downloads/test_group_download.cpp
        downloads/test_mms_download.cpp
        downloads/state_machines/test_cancel_download_transition.cpp
        downloads/state_machines/test_file_download_sm.cpp
        downloads/state_machines/test_final_state.cpp
        downloads/state_machines/test_network_error_transition.cpp
        downloads/state_machines/test_resume_download_transition.cpp
        downloads/state_machines/test_ssl_error_transition.cpp
        downloads/state_machines/test_start_download_transition.cpp
        downloads/state_machines/test_stop_request_transition.cpp
        system/test_apn_request_factory.cpp
        system/test_filename_mutex.cpp
)

set(HEADERS
        downloads/test_base_download.h
        downloads/test_daemon.h
        downloads/test_download.h
        downloads/test_download_factory.h
        downloads/test_download_manager.h
        downloads/test_downloads_db.h
        downloads/test_group_download.h
        downloads/test_mms_download.h
        downloads/state_machines/test_cancel_download_transition.h
        downloads/state_machines/test_file_download_sm.h
        downloads/state_machines/test_final_state.h
        downloads/state_machines/test_network_error_transition.h
        downloads/state_machines/test_resume_download_transition.h
        downloads/state_machines/test_ssl_error_transition.h
        downloads/state_machines/test_start_download_transition.h
        downloads/state_machines/test_stop_request_transition.h
        system/test_apn_request_factory.h
        system/test_filename_mutex.h
)

include_directories(${Qt5SystemInfo_INCLUDE_DIRS})
include_directories(${Qt5DBus_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5Test_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${DBUS_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/udm-common)
include_directories(${CMAKE_SOURCE_DIR}/udm-priv-common)
include_directories(${CMAKE_SOURCE_DIR}/udm-testing)
include_directories(${CMAKE_SOURCE_DIR}/ubuntu-download-manager-common)
include_directories(${CMAKE_SOURCE_DIR}/ubuntu-download-manager-client)
include_directories(${CMAKE_SOURCE_DIR}/ubuntu-download-manager-priv)
include_directories(${CMAKE_SOURCE_DIR}/ubuntu-download-manager-test-lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

link_directories(${NIH_DBUS_LIBDIR})
link_directories(${GLOG_DBUS_LIBDIR})

if(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
add_custom_target(download-daemon-tests-data ALL
        COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
)
endif(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(${TARGET}
	${SOURCES}
        ${HEADERS}
)

target_link_libraries(${TARGET}
	${NIH_DBUS_LIBRARIES}
	${GLOG_LIBRARIES}
        ${Qt5Sql_LIBRARIES}
	${Qt5Test_LIBRARIES}
	udm-common
	udm-priv-common
	udm-testing
	ubuntu-download-manager-common
	ubuntu-download-manager-client
	ubuntu-download-manager-priv
	ubuntu-download-manager-test-lib
)

ADD_TEST(NAME DownloadDaemonTests COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TARGET})

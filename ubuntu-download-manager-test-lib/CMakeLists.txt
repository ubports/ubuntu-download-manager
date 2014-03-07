set(TARGET ubuntu-download-manager-test-lib)

set(SOURCES
        ubuntu/download_manager/tests/client/daemon_testcase.cpp
        ubuntu/download_manager/tests/client/testing_daemon.cpp
        ubuntu/download_manager/tests/client/testing_file_download.cpp
        ubuntu/download_manager/tests/client/testing_interface.cpp
        ubuntu/download_manager/tests/client/testing_manager.cpp
        ubuntu/download_manager/tests/client/testing_manager_adaptor.cpp
        ubuntu/download_manager/tests/server/download.cpp
        ubuntu/download_manager/tests/server/factory.cpp
        ubuntu/download_manager/tests/server/group_download.cpp
        ubuntu/download_manager/tests/server/manager.cpp
        ubuntu/download_manager/tests/server/queue.cpp
        ubuntu/download_manager/tests/server/sm_file_download.cpp
)

set(HEADERS
        ubuntu/download_manager/tests/client/daemon_testcase.h
        ubuntu/download_manager/tests/client/testing_daemon.h
        ubuntu/download_manager/tests/client/testing_file_download.h
        ubuntu/download_manager/tests/client/testing_interface.h
        ubuntu/download_manager/tests/client/testing_manager.h
        ubuntu/download_manager/tests/client/testing_manager_adaptor.h
        ubuntu/download_manager/tests/server/download.h
        ubuntu/download_manager/tests/server/factory.h
        ubuntu/download_manager/tests/server/group_download.h
        ubuntu/download_manager/tests/server/manager.h
        ubuntu/download_manager/tests/server/queue.h
        ubuntu/download_manager/tests/server/sm_file_download.h
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
include_directories(${CMAKE_SOURCE_DIR}/ubuntu-download-manager-priv)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(${TARGET} SHARED
	${HEADERS}
	${SOURCES}
)

link_directories(${NIH_DBUS_LIBDIR})
link_directories(${GLOG_DBUS_LIBDIR})

target_link_libraries(${TARGET}
	${NIH_DBUS_LIBRARIES}
	${GLOG_LIBRARIES}
	${Qt5Test_LIBRARIES}
	udm-common
	udm-priv-common
	udm-testing
	ubuntu-download-manager-common
	ubuntu-download-manager-priv
)

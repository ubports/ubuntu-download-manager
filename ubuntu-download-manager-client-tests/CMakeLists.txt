set(TARGET ubuntu-download-manager-client-tests)

set(SOURCES
        local_tree_testcase.cpp
	main.cpp
        test_download.cpp
        test_download_manager_watch.cpp
        test_download_watch.cpp
        test_group_manager_watch.cpp
        test_manager.cpp
)

set(HEADERS
        local_tree_testcase.h
        test_download.h
        test_download_manager_watch.h
        test_download_watch.h
        test_group_manager_watch.h
        test_manager.h
)

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

add_executable(${TARGET}
	${SOURCES}
        ${HEADERS}
)

target_link_libraries(${TARGET}
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

set(TARGET ubuntu-download-manager-test-daemon)

set(SOURCES
	main.cpp
        testing_daemon.cpp
        testing_file_download.cpp
        testing_interface.cpp
        testing_manager.cpp
        testing_manager_adaptor.cpp
)

set(HEADERS
        testing_daemon.h
        testing_file_download.h
        testing_interface.h
        testing_manager.h
        testing_manager_adaptor.h
)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5DBus_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/common/public)
include_directories(${CMAKE_SOURCE_DIR}/src/common/priv)
include_directories(${CMAKE_SOURCE_DIR}/src/downloads/common)
include_directories(${CMAKE_SOURCE_DIR}/src/downloads/priv)

link_directories(${NIH_DBUS_LIBDIR})
link_directories(${GLOG_DBUS_LIBDIR})

add_executable(${TARGET}
	${SOURCES}
        ${HEADERS}
)

target_link_libraries(${TARGET}
	${GLOG_LIBRARIES}
        ${Qt5Core_LIBRARIES}
        ${Qt5DBus_LIBRARIES}
        ${Qt5Network_LIBRARIES}
	${Qt5Sql_LIBRARIES}
	udm-common
	udm-priv-common
	ubuntu-download-manager-common
	ubuntu-download-manager-priv
)

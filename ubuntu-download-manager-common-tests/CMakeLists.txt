set(TARGET ubuntu-download-common-tests)

set(SOURCES
        main.cpp
        test_metadata.cpp
)

set(HEADERS
        test_metadata.h
)

include_directories(${Qt5SystemInfo_INCLUDE_DIRS})
include_directories(${Qt5DBus_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5Test_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
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
        ${Qt5Sql_LIBRARIES}
	${Qt5Test_LIBRARIES}
	udm-common
	udm-priv-common
	ubuntu-download-manager-common
	ubuntu-download-manager-client
	ubuntu-download-manager-priv
	ubuntu-download-manager-test-lib
)

ADD_TEST(NAME DownloadManagerCommonTests COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TARGET})

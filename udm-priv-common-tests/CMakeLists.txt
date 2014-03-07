set(TARGET udm-priv-common-tests)

set(SOURCES
        main.cpp
        ubuntu/transfers/tests/test_download_queue.cpp
        ubuntu/transfers/tests/transfer.cpp
)

set(HEADERS
        ubuntu/transfers/tests/test_download_queue.h
        ubuntu/transfers/tests/transfer.h
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
	udm-testing
)

add_test(name UdmCommonTests command ${CMAKE_CURRENT_BINARY_DIR}/${TARGET})

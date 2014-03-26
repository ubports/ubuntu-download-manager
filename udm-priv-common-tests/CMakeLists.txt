set(TARGET udm-priv-common-tests)

set(SOURCES
        test_download_queue.cpp
        transfer.cpp
)

set(HEADERS
        test_download_queue.h
        transfer.h
)

include_directories(${Qt5SystemInfo_INCLUDE_DIRS})
include_directories(${Qt5DBus_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5Test_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${DBUS_INCLUDE_DIRS})
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${GMOCK_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/udm-common)
include_directories(${CMAKE_SOURCE_DIR}/udm-priv-common)
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
        ${GMOCK_LIBRARY}
        ${GTEST_BOTH_LIBRARIES}
	udm-common
	udm-priv-common
)

add_test(NAME udm-priv-common-tests COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TARGET})

set(TARGET ubuntu-download-manager-test-lib)

set(SOURCES
        base_testcase.cpp
        daemon_testcase.cpp
        local_tree_testcase.cpp
        testing_interface.cpp
)

set(HEADERS
        apparmor.h
        base_testcase.h
        daemon_testcase.h
        file_manager.h
        local_tree_testcase.h
        matchers.h
        network_reply.h
        process.h
        process_factory.h
        request_factory.h
        system_network_info.h
        testing_interface.h
        uuid_factory.h
)

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

add_library(${TARGET} STATIC
    ${HEADERS}
    ${SOURCES}
)

target_link_libraries(${TARGET}
    ${NIH_DBUS_LIBRARIES}
    ${GLOG_LIBRARIES}
        ${Qt5Core_LIBRARIES}
        ${Qt5Sql_LIBRARIES}
        ${Qt5DBus_LIBRARIES}
    ${Qt5Test_LIBRARIES}
        ${GMOCK_LIBRARY}
        ${GTEST_BOTH_LIBRARIES}
    udm-common
    udm-priv-common
    ubuntu-download-manager-common
    ubuntu-download-manager-client
)

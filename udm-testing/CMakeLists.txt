set (OLD_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
# Don't treat warnings as errors in 3rd_party/{gmock,cucumber-cpp}
string (REPLACE " -Werror " " " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
find_package(Gtest REQUIRED)
include_directories(${GMOCK_INCLUDE_DIR})
set(CMAKE_CXX_FLAGS ${OLD_CMAKE_CXX_FLAGS})

set(TARGET udm-testing)

set(SOURCES
        ubuntu/transfers/tests/base_testcase.cpp
        ubuntu/transfers/tests/fake.cpp
        ubuntu/transfers/tests/system/apn_request_factory.cpp
        ubuntu/transfers/tests/system/apparmor.cpp
        ubuntu/transfers/tests/system/application.cpp
        ubuntu/transfers/tests/system/dbus_connection.cpp
        ubuntu/transfers/tests/system/file_manager.cpp
        ubuntu/transfers/tests/system/filename_mutex.cpp
        ubuntu/transfers/tests/system/network_reply.cpp
        ubuntu/transfers/tests/system/process.cpp
        ubuntu/transfers/tests/system/process_factory.cpp
        ubuntu/transfers/tests/system/qnetwork_reply.cpp
        ubuntu/transfers/tests/system/request_factory.cpp
        ubuntu/transfers/tests/system/system_network_info.cpp
        ubuntu/transfers/tests/system/timer.cpp
        ubuntu/transfers/tests/system/uuid_factory.cpp
)


set(HEADERS
        ubuntu/transfers/tests/base_testcase.h
        ubuntu/transfers/tests/fake.h
        ubuntu/transfers/tests/test_runner.h
        ubuntu/transfers/tests/system/apn_request_factory.h
        ubuntu/transfers/tests/system/apparmor.h
        ubuntu/transfers/tests/system/application.h
        ubuntu/transfers/tests/system/dbus_connection.h
        ubuntu/transfers/tests/system/file_manager.h
        ubuntu/transfers/tests/system/filename_mutex.h
        ubuntu/transfers/tests/system/network_reply.h
        ubuntu/transfers/tests/system/process.h
        ubuntu/transfers/tests/system/process_factory.h
        ubuntu/transfers/tests/system/qnetwork_reply.h
        ubuntu/transfers/tests/system/request_factory.h
        ubuntu/transfers/tests/system/system_network_info.h
        ubuntu/transfers/tests/system/timer.h
        ubuntu/transfers/tests/system/uuid_factory.h
)

include_directories(${Qt5SystemInfo_INCLUDE_DIRS})
include_directories(${Qt5Test_INCLUDE_DIRS})
include_directories(${Qt5DBus_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/udm-common)
include_directories(${CMAKE_SOURCE_DIR}/udm-priv-common)
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(${TARGET} SHARED
	${SOURCES}
	${HEADERS}
)

target_link_libraries(${TARGET}
	${Qt5DBus_LIBRARIES}
        ${Qt5SystemInfo_LIBRARIES}
	${Qt5Test_LIBRARIES}
	${GMOCK_LIBRARY}
        udm-common
        udm-priv-common
)

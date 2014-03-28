set(TARGET udm-priv-common)

set(SOURCES
	ubuntu/transfers/base_daemon.cpp
	ubuntu/transfers/base_manager.cpp
	ubuntu/transfers/queue.cpp
	ubuntu/transfers/transfer.cpp
	ubuntu/transfers/system/apn_proxy.cpp
	ubuntu/transfers/system/apn_request_factory.cpp
	ubuntu/transfers/system/apparmor.cpp
	ubuntu/transfers/system/application.cpp
        ubuntu/transfers/system/cryptographic_hash.cpp
	ubuntu/transfers/system/dbus_proxy.cpp
	ubuntu/transfers/system/file_manager.cpp
	ubuntu/transfers/system/filename_mutex.cpp
	ubuntu/transfers/system/network_reply.cpp
	ubuntu/transfers/system/process.cpp
	ubuntu/transfers/system/process_factory.cpp
	ubuntu/transfers/system/request_factory.cpp
	ubuntu/transfers/system/system_network_info.cpp
	ubuntu/transfers/system/timer.cpp
	ubuntu/transfers/system/uuid_factory.cpp
	ubuntu/transfers/system/uuid_utils.cpp
)

set(HEADERS
	ubuntu/transfers/adaptor_factory.h
	ubuntu/transfers/base_daemon.h
	ubuntu/transfers/base_manager.h
	ubuntu/transfers/manager_factory.h
	ubuntu/transfers/queue.h
	ubuntu/transfers/transfer.h
	ubuntu/transfers/system/apn_proxy.h
	ubuntu/transfers/system/apn_request_factory.h
	ubuntu/transfers/system/apparmor.h
	ubuntu/transfers/system/application.h
        ubuntu/transfers/system/cryptographic_hash.h
	ubuntu/transfers/system/dbus_proxy.h
	ubuntu/transfers/system/file_manager.h
	ubuntu/transfers/system/filename_mutex.h
	ubuntu/transfers/system/network_reply.h
	ubuntu/transfers/system/process.h
	ubuntu/transfers/system/process_factory.h
	ubuntu/transfers/system/request_factory.h
	ubuntu/transfers/system/system_network_info.h
	ubuntu/transfers/system/timer.h
	ubuntu/transfers/system/uuid_factory.h
	ubuntu/transfers/system/uuid_utils.h
)

include_directories(${Qt5SystemInfo_INCLUDE_DIRS})
include_directories(${Qt5DBus_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${DBUS_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/udm-common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(${TARGET} SHARED
	${HEADERS}
	${SOURCES}
)

set_target_properties(
	${TARGET}

	PROPERTIES
	VERSION ${UDM_VERSION_MAJOR}.${UDM_VERSION_MINOR}.${UDM_VERSION_PATCH}
	SOVERSION ${UDM_VERSION_MAJOR}
)

link_directories(${NIH_DBUS_LIBDIR})
link_directories(${GLOG_DBUS_LIBDIR})

target_link_libraries(${TARGET}
	${NIH_DBUS_LIBRARIES}
	${GLOG_LIBRARIES}
	${Qt5Network_LIBRARIES}
	${Qt5Sql_LIBRARIES}
	${Qt5SystemInfo_LIBRARIES}
	${Qt5Core_LIBRARIES}
	${Qt5DBus_LIBRARIES}
	udm-common
)

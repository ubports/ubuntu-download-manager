set(TARGET ubuntu-download-manager-priv)

set(SOURCES
	ubuntu/downloads/daemon.cpp
	ubuntu/downloads/download.cpp
	ubuntu/downloads/download_adaptor.cpp
	ubuntu/downloads/download_adaptor_factory.cpp
	ubuntu/downloads/download_manager_adaptor.cpp
	ubuntu/downloads/download_manager_factory.cpp
	ubuntu/downloads/downloads_db.cpp
	ubuntu/downloads/factory.cpp
	ubuntu/downloads/file_download.cpp
	ubuntu/downloads/group_download.cpp
	ubuntu/downloads/group_download_adaptor.cpp
	ubuntu/downloads/header_parser.cpp
	ubuntu/downloads/manager.cpp
	ubuntu/downloads/mms_file_download.cpp
	ubuntu/downloads/sm_file_download.cpp
	ubuntu/downloads/state_machines/download_sm.cpp
	ubuntu/downloads/state_machines/final_state.cpp
	ubuntu/downloads/state_machines/state.cpp
)

set(HEADERS
	ubuntu/downloads/daemon.h
	ubuntu/downloads/download.h
	ubuntu/downloads/download_adaptor.h
	ubuntu/downloads/download_adaptor_factory.h
	ubuntu/downloads/download_manager_adaptor.h
	ubuntu/downloads/download_manager_factory.h
	ubuntu/downloads/downloads_db.h
	ubuntu/downloads/factory.h
	ubuntu/downloads/file_download.h
	ubuntu/downloads/group_download.h
	ubuntu/downloads/group_download_adaptor.h
	ubuntu/downloads/header_parser.h
	ubuntu/downloads/manager.h
	ubuntu/downloads/mms_file_download.h
	ubuntu/downloads/sm_file_download.h
	ubuntu/downloads/state_machines/download_sm.h
	ubuntu/downloads/state_machines/final_state.h
	ubuntu/downloads/state_machines/state.h
)

include_directories(${Qt5DBus_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${DBUS_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/common/public)
include_directories(${CMAKE_SOURCE_DIR}/src/common/priv)
include_directories(${CMAKE_SOURCE_DIR}/src/downloads/common)

add_definitions(-DHELPER_DIR="${CMAKE_INSTALL_LIBEXECDIR}/ubuntu-download-manager")

add_library(${TARGET} STATIC
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
	${Qt5DBus_LIBRARIES}
	${Qt5Sql_LIBRARIES}
	udm-common
	udm-priv-common
	ubuntu-download-manager-common
)

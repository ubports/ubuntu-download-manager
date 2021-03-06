set(TARGET ubuntu-upload-manager-priv)

set(SOURCES
        ubuntu/uploads/daemon.cpp
        ubuntu/uploads/factory.cpp
	ubuntu/uploads/manager.cpp
        ubuntu/uploads/file_upload.cpp
        ubuntu/uploads/manager.cpp
        ubuntu/uploads/mms_file_upload.cpp
        ubuntu/uploads/upload_adaptor.cpp
        ubuntu/uploads/upload_adaptor_factory.cpp
        ubuntu/uploads/upload_manager_adaptor.cpp
        ubuntu/uploads/upload_manager_factory.cpp
)

set(HEADERS
        ubuntu/uploads/daemon.h
        ubuntu/uploads/factory.h
	ubuntu/uploads/manager.h
        ubuntu/uploads/file_upload.h
        ubuntu/uploads/manager.h
        ubuntu/uploads/mms_file_upload.h
        ubuntu/uploads/upload_adaptor.h
        ubuntu/uploads/upload_adaptor_factory.h
        ubuntu/uploads/upload_manager_adaptor.h
        ubuntu/uploads/upload_manager_factory.h
)

include_directories(${Qt5DBus_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${DBUS_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/common/public)
include_directories(${CMAKE_SOURCE_DIR}/src/common/priv)
include_directories(${CMAKE_SOURCE_DIR}/src/uploads/common)

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
	ubuntu-upload-manager-common
)

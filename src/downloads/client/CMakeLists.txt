set(TARGET ubuntu-download-manager-client)

set(SOURCES
        ubuntu/download_manager/download_impl.cpp
        ubuntu/download_manager/download_interface.cpp
	ubuntu/download_manager/downloads_list_impl.cpp
        ubuntu/download_manager/download_pendingcall_watcher.cpp
        ubuntu/download_manager/error.cpp
        ubuntu/download_manager/group_download.cpp
        ubuntu/download_manager/manager.cpp
        ubuntu/download_manager/manager_impl.cpp
        ubuntu/download_manager/manager_interface.cpp
        ubuntu/download_manager/manager_pendingcall_watcher.cpp
        ubuntu/download_manager/pending_call_watcher.cpp
)

set(PUBLIC_HEADERS
        ubuntu/download_manager/download.h
	ubuntu/download_manager/downloads_list.h
        ubuntu/download_manager/error.h
        ubuntu/download_manager/group_download.h
        ubuntu/download_manager/manager.h
)

set(PRIVATE_HEADERS
        ubuntu/download_manager/download_impl.h
        ubuntu/download_manager/download_interface.h
	ubuntu/download_manager/downloads_list_impl.h
        ubuntu/download_manager/download_pendingcall_watcher.h
        ubuntu/download_manager/manager_impl.h
        ubuntu/download_manager/manager_interface.h
        ubuntu/download_manager/manager_pendingcall_watcher.h
        ubuntu/download_manager/pending_call_watcher.h
)

include_directories(${Qt5DBus_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/common/public)
include_directories(${CMAKE_SOURCE_DIR}/src/downloads/common)

add_library(${TARGET} SHARED 
	${SOURCES}
	${PUBLIC_HEADERS}
	${PRIVATE_HEADERS}
)

set_target_properties(
	${TARGET}

	PROPERTIES
	VERSION ${UDM_VERSION_MAJOR}.${UDM_VERSION_MINOR}.${UDM_VERSION_PATCH}
	SOVERSION ${UDM_VERSION_MAJOR}
)

target_link_libraries(${TARGET}
	${Qt5DBus_LIBRARIES}
	${GLOG_LIBRARIES}
	udm-common
        ubuntu-download-manager-common
)

configure_file(${TARGET}.pc.in ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.pc DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/pkgconfig)
install(TARGETS ${TARGET} DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ubuntu/download_manager)

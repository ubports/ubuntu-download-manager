set(TARGET ubuntu-download-manager)

set(SOURCES
	main.cpp
)

include_directories(${Qt5Core_INCLUDE_DIRS})
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
)

target_link_libraries(${TARGET}
	${GLOG_LIBRARIES}
        ${Qt5Core_LIBRARIES}
	udm-common
	udm-priv-common
	ubuntu-download-manager-common
	ubuntu-download-manager-priv
)

install(FILES ${TARGET}.service DESTINATION
   	${CMAKE_INSTALL_DATADIR}/dbus-1/services)
install(FILES "com.canonical.applications.Downloader.service" DESTINATION
   	/etc/dbus-1/system-services)
install(FILES "com.canonical.applications.Downloader.conf" DESTINATION
   	/etc/dbus-1/system.d)
install(TARGETS ${TARGET} DESTINATION ${CMAKE_INSTALL_BINDIR})

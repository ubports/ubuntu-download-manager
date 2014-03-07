set(TARGET ubuntu-download-manager)

set(SOURCES
	main.cpp
)

include_directories(${Qt5SystemInfo_INCLUDE_DIRS})
include_directories(${Qt5DBus_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${DBUS_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/udm-common)
include_directories(${CMAKE_SOURCE_DIR}/udm-priv-common)
include_directories(${CMAKE_SOURCE_DIR}/ubuntu-download-manager-common)
include_directories(${CMAKE_SOURCE_DIR}/ubuntu-download-manager-priv)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

link_directories(${NIH_DBUS_LIBDIR})
link_directories(${GLOG_DBUS_LIBDIR})

add_executable(${TARGET}
	${SOURCES}
)

target_link_libraries(${TARGET}
	${NIH_DBUS_LIBRARIES}
	${GLOG_LIBRARIES}
	udm-common
	udm-priv-common
	ubuntu-download-manager-common
	ubuntu-download-manager-priv
)

install(FILES ${TARGET}.service DESTINATION
   	${CMAKE_INSTALL_DATADIR}/dbus-1/system-services)
install(FILES "com.canonical.applications.Downloader.conf" DESTINATION
   	${CMAKE_INSTALL_SYSCONDIF}/dbus-1/system.d)
install(TARGETS ${TARGET} DESTINATION ${CMAKE_INSTALL_BINDIR})

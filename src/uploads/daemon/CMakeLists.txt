set(TARGET ubuntu-upload-manager)

set(SOURCES
	main.cpp
)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/common/public)
include_directories(${CMAKE_SOURCE_DIR}/src/common/priv)
include_directories(${CMAKE_SOURCE_DIR}/src/uploads/common)
include_directories(${CMAKE_SOURCE_DIR}/src/uploads/priv)

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
	ubuntu-upload-manager-common
	ubuntu-upload-manager-priv
)

install(FILES ${TARGET}.service DESTINATION
   	${CMAKE_INSTALL_DATADIR}/dbus-1/services)
install(FILES "com.canonical.applications.Uploader.service" DESTINATION
   	${CMAKE_INSTALL_SYSCONDIF}/dbus-1/system-services)
install(FILES "com.canonical.applications.Uploader.conf" DESTINATION
   	${CMAKE_INSTALL_DATADIR}/dbus-1/system.d)
install(TARGETS ${TARGET} DESTINATION ${CMAKE_INSTALL_BINDIR})

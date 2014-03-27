pkg_check_modules(GLOG libglog)

set(TARGET udm-common)

set(SOURCES
	ubuntu/transfers/metadata.cpp
	ubuntu/transfers/errors/auth_error_struct.cpp
	ubuntu/transfers/errors/http_error_struct.cpp
	ubuntu/transfers/errors/network_error_struct.cpp
	ubuntu/transfers/errors/process_error_struct.cpp
	ubuntu/transfers/system/dbus_connection.cpp
	ubuntu/transfers/system/hash_algorithm.cpp
	ubuntu/transfers/system/logger.cpp
)

set(PUBLIC_HEADERS
	ubuntu/transfers/metadata.h
	ubuntu/transfers/errors/auth_error_struct.h
	ubuntu/transfers/errors/http_error_struct.h
	ubuntu/transfers/errors/network_error_struct.h
	ubuntu/transfers/errors/process_error_struct.h
)

set(PRIVATE_HEADERS
	ubuntu/transfers/system/dbus_connection.h
	ubuntu/transfers/system/hash_algorithm.h
	ubuntu/transfers/system/logger.h
)

include_directories(${Qt5DBus_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})

add_library(${TARGET} SHARED
	${SOURCES}
	${PUBLIC_HEADERS}
	${PRIVATE_HEADERS}
)

target_link_libraries(${TARGET}
	${Qt5DBus_LIBRARIES}
	${Qt5Network_LIBRARIES}
	glog
)

configure_file(${TARGET}.pc.in ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.pc)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.pc DESTINATION ${CMAKE_INSTALL_DATADIR}/pkgconfig)
install(TARGETS ${TARGET} DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ubuntu/transfers/errors)

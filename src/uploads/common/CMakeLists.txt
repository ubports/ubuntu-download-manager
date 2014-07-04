set(TARGET ubuntu-upload-manager-common)

set(SOURCES
        ubuntu/upload_manager/upload_struct.cpp
)

set(PUBLIC_HEADERS
        ubuntu/upload_manager/common.h
        ubuntu/upload_manager/metatypes.h
        ubuntu/upload_manager/upload_struct.h
)

set(PRIVATE_HEADERS
)

include_directories(${Qt5DBus_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/common/public)

add_library(${TARGET} SHARED 
	${SOURCES}
	${PUBLIC_HEADERS}
	${PRIVATE_HEADERS}
)

set(symbol_map "${CMAKE_SOURCE_DIR}/src/uploads/common/symbols.map")
set_target_properties(
	${TARGET}

	PROPERTIES
        LINK_FLAGS "${ldflags} -Wl,--version-script,${symbol_map}"
        LINK_DEPENDS ${symbol_map}
	VERSION ${UDM_VERSION_MAJOR}.${UDM_VERSION_MINOR}.${UDM_VERSION_PATCH}
	SOVERSION ${UDM_VERSION_MAJOR}
)

target_link_libraries(${TARGET}
	${Qt5DBus_LIBRARIES}
	udm-common
)

configure_file(${TARGET}.pc.in ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.pc DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/pkgconfig)
install(TARGETS ${TARGET} DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ubuntu/upload_manager)

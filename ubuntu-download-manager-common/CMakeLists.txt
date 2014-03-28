set(TARGET ubuntu-download-manager-common)

set(SOURCES
	ubuntu/download_manager/download_struct.cpp
	ubuntu/download_manager/group_download_struct.cpp
	ubuntu/download_manager/system/logger.cpp
)

set(PUBLIC_HEADERS
	ubuntu/download_manager/common.h
	ubuntu/download_manager/download_struct.h
	ubuntu/download_manager/group_download_struct.h
	ubuntu/download_manager/metatypes.h
)

set(PRIVATE_HEADERS
	ubuntu/download_manager/system/logger.h
)

include_directories(${Qt5DBus_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/udm-common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(${TARGET} SHARED 
	${SOURCES}
	${PUBLIC_HEADERS}
	${PRIVATE_HEADERS}
)

target_link_libraries(${TARGET}
	${Qt5DBus_LIBRARIES}
	udm-common
)

configure_file(${TARGET}.pc.in ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.pc DESTINATION ${CMAKE_INSTALL_DATADIR}/pkgconfig)
install(TARGETS ${TARGET} DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ubuntu/download_manager)

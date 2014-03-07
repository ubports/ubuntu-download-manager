set(TARGET ubuntu-download-manager-test-daemon)

set(SOURCES
	main.cpp
)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/udm-common)
include_directories(${CMAKE_SOURCE_DIR}/udm-priv-common)
include_directories(${CMAKE_SOURCE_DIR}/udm-testing)
include_directories(${CMAKE_SOURCE_DIR}/ubuntu-download-manager-common)
include_directories(${CMAKE_SOURCE_DIR}/ubuntu-download-manager-priv)
include_directories(${CMAKE_SOURCE_DIR}/ubuntu-download-manager-test-lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

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
	udm-testing
	ubuntu-download-manager-common
	ubuntu-download-manager-priv
        ubuntu-download-manager-test-lib
)

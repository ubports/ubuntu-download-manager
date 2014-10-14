set(TARGET udm-extractor)

set(SOURCES
    ubuntu/downloads/extractor/deflator.cpp
    ubuntu/downloads/extractor/factory.cpp
    ubuntu/downloads/extractor/main.cpp
    ubuntu/downloads/extractor/unzip.cpp
)

set(HEADERS
    ubuntu/downloads/extractor/deflator.h
    ubuntu/downloads/extractor/factory.h
    ubuntu/downloads/extractor/unzip.h
)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/common/priv)

add_executable(${TARGET}
	${HEADERS}
	${SOURCES}
)

target_link_libraries(${TARGET}
	${GLOG_LIBRARIES}
        ${Qt5Core_LIBRARIES}
        ${Boost_LIBRARIES}
	udm-priv-common
)

install(TARGETS ${TARGET} DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/ubuntu-download-manager)

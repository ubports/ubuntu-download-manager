# QML plugin
set(TARGET UbuntuDownloadManager)

set(plugin_SRCS
    backend.cpp
    download_error.cpp
    single_download.cpp
    ubuntu_download_manager.cpp
)

set(plugin_HDRS
    backend.h
    download_error.h
    single_download.h
    ubuntu_download_manager.h
)

include_directories(${CMAKE_SOURCE_DIR}/src/common/public)
include_directories(${CMAKE_SOURCE_DIR}/src/downloads/common)
include_directories(${CMAKE_SOURCE_DIR}/src/downloads/client)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(${TARGET} SHARED
	${plugin_SRCS}
	${plugin_HDRS}
)

qt5_use_modules(${TARGET} Core Qml Quick)

target_link_libraries(${TARGET}
	${GLOG_LIBRARIES}
	udm-common
	ubuntu-download-manager-common
	ubuntu-download-manager-client
)

set(PLUGIN_DIR ${QT_INSTALL_QML}/Ubuntu/DownloadManager)
install(TARGETS ${TARGET} DESTINATION ${PLUGIN_DIR})
install(FILES qmldir DESTINATION ${PLUGIN_DIR})

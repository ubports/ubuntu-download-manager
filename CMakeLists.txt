project(ubuntu-download-manager)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -pthread")


# Standard install paths
include(GNUInstallDirs)

include(EnableCoverageReport)
#####################################################################
# Enable code coverage calculation with gcov/gcovr/lcov
# Usage:
#  * Switch build type to coverage (use ccmake or cmake-gui)
#  * Invoke make, make test, make coverage
#  * Find html report in subdir coveragereport
#  * Find xml report feasible for jenkins in coverage.xml
#####################################################################
IF(CMAKE_BUILD_TYPE MATCHES [cC][oO][vV][eE][rR][aA][gG][eE])
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftest-coverage -fprofile-arcs" )
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ftest-coverage -fprofile-arcs" )
  SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -coverage" )
  SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -coverage" )
  ENABLE_COVERAGE_REPORT(TARGETS ${DIALER_APP})
ENDIF(CMAKE_BUILD_TYPE MATCHES [cC][oO][vV][eE][rR][aA][gG][eE])

execute_process(
    COMMAND qmake -query QT_INSTALL_QML
    OUTPUT_VARIABLE QT_INSTALL_QML
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(UDM_VERSION_MAJOR 0)
set(UDM_VERSION_MINOR 3)
set(UDM_VERSION_PATCH 0)

find_package(Gtest REQUIRED)
include_directories(${GMOCK_INCLUDE_DIR} ${GTEST_INCLUDE_DIR})

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5SystemInfo REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Test REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(DBUS REQUIRED dbus-1)
pkg_check_modules(GLOG REQUIRED libglog)
pkg_check_modules(NIH_DBUS REQUIRED libnih-dbus)
pkg_check_modules(GLOG REQUIRED libglog)
pkg_check_modules(GLOG libglog)

enable_testing()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -std=c++11 -Werror -O2 -Wall -W -D_REENTRANT -fPIC -pedantic -Wextra")
add_definitions("-DNDEBUG")

if(NOT CMAKE_BUILD_TYPE)
	message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
	set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_lower)

if(cmake_build_type_lower MATCHES relwithdebinfo) # workaround for http://pub    lic.kitware.com/Bug/view.php?id=14696
	add_definitions(-DQT_NO_DEBUG)
endif()

add_subdirectory(udm-common)
add_subdirectory(udm-priv-common)
add_subdirectory(ubuntu-download-manager-common)
add_subdirectory(ubuntu-download-manager-priv)
add_subdirectory(ubuntu-download-manager)
add_subdirectory(ubuntu-download-manager-client)
add_subdirectory(ubuntu-download-manager-test-daemon)
add_subdirectory(ubuntu-download-manager-plugin)
add_subdirectory(ubuntu-upload-manager-common)
add_subdirectory(ubuntu-upload-manager-priv)
add_subdirectory(ubuntu-upload-manager)
add_subdirectory(tests)

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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -pedantic -Wextra")
add_definitions("-DNDEBUG")

add_subdirectory(udm-common)
add_subdirectory(udm-priv-common)
add_subdirectory(ubuntu-download-manager-common)
add_subdirectory(ubuntu-download-manager-priv)
add_subdirectory(ubuntu-download-manager)
add_subdirectory(ubuntu-download-manager-client)
add_subdirectory(ubuntu-download-manager-test-daemon)
add_subdirectory(ubuntu-upload-manager-common)
add_subdirectory(ubuntu-upload-manager-priv)
add_subdirectory(ubuntu-upload-manager)
add_subdirectory(tests)

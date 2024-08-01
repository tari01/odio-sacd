cmake_minimum_required (VERSION 3.13)
project (odio-sacd VERSION 24.8.1 LANGUAGES C)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET (CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "..." FORCE)
endif (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# Options

option (ENABLE_WERROR "Treat all build warnings as errors" OFF)

set(CMAKE_BUILD_TYPE "Release")

if (ENABLE_WERROR)
    add_definitions ("-Werror")
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_definitions ("-Weverything")
else ()
    add_definitions ("-Wall")
endif ()

# Prerequisites

include (GNUInstallDirs)
find_package (PkgConfig REQUIRED)
pkg_check_modules (DEPS REQUIRED libodiosacd)

# Build

add_subdirectory (src)
add_subdirectory (man)

# Info

message (STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message (STATUS "Build with -Werror: ${ENABLE_WERROR}")

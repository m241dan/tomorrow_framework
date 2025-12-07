cmake_minimum_required(VERSION 3.28)

project(tomorrow_framework)

set(CMAKE_CXX_STANDARD 23)

# download CPM.cmake

file(
    DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.3/CPM.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
        EXPECTED_HASH SHA256=cc155ce02e7945e7b8967ddfaff0b050e958a723ef7aad3766d368940cb15494
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

CPMAddPackage(
    NAME Boost
    VERSION 1.86.0
    URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.tar.xz
    OPTIONS "BOOST_ENABLE_CMAKE ON"
)
CPMAddPackage("gh:fmtlib/fmt#11.1.0")

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    CPMAddPackage("gh:catchorg/Catch2@3.4.0")
endif()

add_subdirectory(src)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(test)
endif()

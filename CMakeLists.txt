cmake_minimum_required(VERSION 3.20)

#[[
ASE FOUNDATION BUILD CONFIGURATION

@file        CMakeLists.txt
@brief       Build configuration for ase-platform
@description OS-abstracted primitives (Thread, time, SIMD, system info).
             Layer 0 foundation — no ASE dependencies.

@module      ase-platform
@layer       0 (Foundation)
@created     2025-12-15
@modified    2026-04-16
@version     00.00.11.00011 [init]
]]

# Parse local VERSION (SSOT) if not building from root
if(NOT DEFINED ASE_PLATFORM_VERSION)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
        message(FATAL_ERROR "VERSION file not found: ${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
    endif()
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" _VERSION_LINES)
    foreach(_LINE ${_VERSION_LINES})
        if(_LINE MATCHES "^ASE_PLATFORM=(.*)$")
            set(ASE_PLATFORM_VERSION "${CMAKE_MATCH_1}")
        endif()
    endforeach()
    if(NOT DEFINED ASE_PLATFORM_VERSION)
        message(FATAL_ERROR "ASE_PLATFORM version not found in VERSION file")
    endif()
endif()

project(ase-platform VERSION ${ASE_PLATFORM_VERSION} LANGUAGES CXX)

# Generate version.hpp from template (SSOT)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ase/platform/version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/ase/platform/version.hpp"
    @ONLY
)

# ==============================================================================
# EXTERNAL DEPENDENCIES
# Threads::Threads resolves to pthread on POSIX and to the Win32 threading
# model on Windows. Platform specifics stay hidden behind a PIMPL in
# src/thread.cpp and never leak through the public header.
# ==============================================================================
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# ==============================================================================
# TARGET
# ==============================================================================
set(ASE_PLATFORM_PUBLIC_HEADERS
    include/ase/platform/thread.hpp
    "${CMAKE_CURRENT_BINARY_DIR}/include/ase/platform/version.hpp"
)

set(ASE_PLATFORM_SOURCES
    src/thread.cpp
)

add_library(${PROJECT_NAME} STATIC ${ASE_PLATFORM_SOURCES})
add_library(ase::platform ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Threads::Threads
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    PUBLIC_HEADER "${ASE_PLATFORM_PUBLIC_HEADERS}"
)

message(STATUS "Configuring ${PROJECT_NAME} v${PROJECT_VERSION} (init)")

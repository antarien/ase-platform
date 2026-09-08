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
@modified    2026-09-08
@version     00.00.13.00013 [poc]

CMAKE MODULE COMPLIANCE

[ ] cmake_minimum_required(VERSION 3.20) as FIRST line
[ ] project() with VERSION and LANGUAGES CXX
[ ] message(STATUS) for configuration output
[ ] add_library() with STATIC (compiled unit)
[ ] add_library(ase::{module} ALIAS ...) for namespace
[ ] target_include_directories() with PUBLIC and generator expressions
[ ] target_compile_features() with cxx_std_20
[ ] target_link_libraries() with Threads::Threads (Layer 0 allowed external)
[ ] if(ASE_BUILD_TESTS) block for tests
[ ] include(GNUInstallDirs) for installation
[ ] install(DIRECTORY include/ ...) for headers
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
#[[ Diese Liste ist die SSOT dessen, was das Modul veroeffentlicht. sleep.hpp fehlte hier,
    obwohl core/ase-ecs sie an drei Stellen einbindet (app.cpp, internal/boot_logger.cpp,
    internal/shutdown_sequence.cpp) — eine nicht eingetragene Datei faellt aus der Installation
    und meldet das nie, weil der Bau gruen bleibt. ]]
set(ASE_PLATFORM_PUBLIC_HEADERS
    include/ase/platform/thread.hpp
    include/ase/platform/sleep.hpp
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
#[[ VERBOTEN: No ASE dependencies in Layer 0! Only external libs allowed!
    Threads::Threads ist genau so eine: sie kommt von der Toolchain, nicht aus diesem Baum.
    Eine ase::-Kante waere hier auch dann falsch, wenn das Ziel tiefer laege — L0 ist der
    Boden, unter dem nichts mehr ist. ]]

set_target_properties(${PROJECT_NAME} PROPERTIES
    PUBLIC_HEADER "${ASE_PLATFORM_PUBLIC_HEADERS}"
)

message(STATUS "Configuring ${PROJECT_NAME} v${PROJECT_VERSION} (init)")

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

#[[ Die Existenzpruefung ist notwendig, nicht vorsichtig: dieses Modul hat noch kein
    tests/-Verzeichnis, und ein add_subdirectory auf ein fehlendes Verzeichnis bricht die
    Konfiguration des GANZEN Baums. Sobald ein Test entsteht, greift der Zweig von selbst. ]]
if(ASE_BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    add_subdirectory(tests)
endif()

cmake_minimum_required(VERSION 3.20)

#[[
ASE FOUNDATION BUILD CONFIGURATION (STUB)

@file        CMakeLists.txt
@brief       Build configuration for ase-platform
@description Platform abstraction - OS-specific functionality.
             (STUB - Not yet implemented)

@module      ase-platform
@layer       0 (Foundation)
@created     2025-12-15
@modified    2026-01-30
@version     00.00.05.00005 [poc]

CMAKE MODULE COMPLIANCE

[ ] cmake_minimum_required(VERSION 3.20) as FIRST line
[ ] project() with VERSION and LANGUAGES CXX
[ ] message(STATUS) for configuration output
[ ] Stub target until implementation
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

# ==============================================================================
# BUILD CONFIGURATION
# ==============================================================================



message(STATUS "Configuring ${PROJECT_NAME} v${PROJECT_VERSION} (STUB)")

#[[
STUB MODULE
Platform abstraction layer - not yet implemented.
]]
add_custom_target(${PROJECT_NAME}_stub
    COMMAND ${CMAKE_COMMAND} -E echo "Module ${PROJECT_NAME} is a stub"
)

#[[ VERBOTEN: No dependencies until implemented! ]]

cmake_minimum_required(VERSION 3.20)
project(ase-platform VERSION 0.1.0 LANGUAGES CXX)

#[[
ASE FOUNDATION BUILD CONFIGURATION (STUB)

@file        CMakeLists.txt
@brief       Build configuration for ase-platform
@description Platform abstraction - OS-specific functionality.
             (STUB - Not yet implemented)

@module      ase-platform
@layer       0 (Foundation)
@created     2025-01-01
@modified    2025-01-30
@version     0.1.0

CMAKE MODULE COMPLIANCE

[ ] cmake_minimum_required(VERSION 3.20) as FIRST line
[ ] project() with VERSION and LANGUAGES CXX
[ ] message(STATUS) for configuration output
[ ] Stub target until implementation
]]

message(STATUS "Configuring ${PROJECT_NAME} v${PROJECT_VERSION} (STUB)")

#[[
STUB MODULE
Platform abstraction layer - not yet implemented.
]]
add_custom_target(${PROJECT_NAME}_stub
    COMMAND ${CMAKE_COMMAND} -E echo "Module ${PROJECT_NAME} is a stub"
)

#[[ VERBOTEN: No dependencies until implemented! ]]

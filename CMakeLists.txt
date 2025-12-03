# =============================================================================
# CMakeLists.txt for proyectoDCA - Juego de Plataformas
# =============================================================================
# Modern CMake configuration for multi-directory C++ project with Raylib
# =============================================================================

cmake_minimum_required(VERSION 3.15)

# Project definition
project(proyectoDCA
    VERSION 1.0.0
    DESCRIPTION "Juego de plataformas con Raylib"
    LANGUAGES CXX
)

# =============================================================================
# Build Configuration
# =============================================================================

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "ccache found: ${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
else()
    message(STATUS "ccache not found - compilation will not be cached")
endif()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# =============================================================================
# Dependencies
# =============================================================================

# Check if we're building inside Flatpak or using system/vendor libraries
if(DEFINED ENV{FLATPAK_ID})
    message(STATUS "Building inside Flatpak - using system Raylib")
    find_package(raylib 5.0 REQUIRED)
else()
    message(STATUS "Building outside Flatpak - using vendor Raylib")
    
    # Add vendor library directory
    set(VENDOR_LIB_DIR "${CMAKE_SOURCE_DIR}/vendor/lib")
    set(VENDOR_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/vendor/include")
    
    if(EXISTS "${VENDOR_LIB_DIR}" AND EXISTS "${VENDOR_INCLUDE_DIR}")
        # Check for static or shared library
        if(EXISTS "${VENDOR_LIB_DIR}/libraylib.a")
            # Create imported target for static Raylib
            add_library(raylib STATIC IMPORTED)
            set_target_properties(raylib PROPERTIES
                IMPORTED_LOCATION "${VENDOR_LIB_DIR}/libraylib.a"
                INTERFACE_INCLUDE_DIRECTORIES "${VENDOR_INCLUDE_DIR}"
            )
            message(STATUS "Using vendor Raylib (static library)")
        elseif(EXISTS "${VENDOR_LIB_DIR}/libraylib.so")
            # Create imported target for shared Raylib
            add_library(raylib SHARED IMPORTED)
            set_target_properties(raylib PROPERTIES
                IMPORTED_LOCATION "${VENDOR_LIB_DIR}/libraylib.so"
                INTERFACE_INCLUDE_DIRECTORIES "${VENDOR_INCLUDE_DIR}"
            )
            message(STATUS "Using vendor Raylib (shared library)")
            # Add vendor lib to runtime path
            link_directories("${VENDOR_LIB_DIR}")
        else()
            message(FATAL_ERROR "No Raylib library found in ${VENDOR_LIB_DIR}")
        endif()
    else()
        message(FATAL_ERROR "Vendor libraries not found. Please ensure vendor/lib and vendor/include exist.")
    endif()

endif()

# System dependencies
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

# =============================================================================
# Source Files
# =============================================================================

# Automatically collect all source files
file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

# Automatically collect all header files (for IDE support)
file(GLOB_RECURSE HEADERS
    "${CMAKE_SOURCE_DIR}/src/*.h"
    "${CMAKE_SOURCE_DIR}/src/*.hpp"
)

# Display found sources
message(STATUS "Found ${CMAKE_CURRENT_LIST_LINE} source files:")
foreach(source ${SOURCES})
    message(STATUS "  - ${source}")
endforeach()

# =============================================================================
# Executable Target
# =============================================================================

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    raylib
    OpenGL::GL
    ${CMAKE_DL_LIBS}
    m
    Threads::Threads
    rt
    X11
)

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# =============================================================================
# Installation Rules
# =============================================================================

# Install executable
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Install assets
install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets
    DESTINATION share/${PROJECT_NAME}
    OPTIONAL
)

# =============================================================================
# Custom Targets
# =============================================================================

# Custom target for running the game
add_custom_target(run
    COMMAND ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running ${PROJECT_NAME}..."
)

# Custom target for ccache statistics
if(CCACHE_PROGRAM)
    add_custom_target(ccache-stats
        COMMAND ${CCACHE_PROGRAM} -s
        COMMENT "Displaying ccache statistics..."
    )
    
    add_custom_target(ccache-clear
        COMMAND ${CCACHE_PROGRAM} -C
        COMMENT "Clearing ccache..."
    )
endif()
# =============================================================================
# Copy Assets to Build Directory
# =============================================================================

# Copy assets folder to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/assets
    COMMENT "Copying assets to build directory..."
)

# =============================================================================
# Development Info
# =============================================================================

message(STATUS "=============================================================================")
message(STATUS "Configuration Summary:")
message(STATUS "  Project:          ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "  Build type:       ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard:     C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler:         ${CMAKE_CXX_COMPILER}")
message(STATUS "  Compiler cache:   ${CCACHE_PROGRAM}")
message(STATUS "  Source files:     ${CMAKE_CURRENT_LIST_LINE} files")
message(STATUS "  Install prefix:   ${CMAKE_INSTALL_PREFIX}")
message(STATUS "=============================================================================")
message(STATUS "Build targets:")
message(STATUS "  ${PROJECT_NAME}   - Build the game executable")
message(STATUS "  run               - Build and run the game")
message(STATUS "  install           - Install the game")
if(CCACHE_PROGRAM)
message(STATUS "  ccache-stats      - Show ccache statistics")
message(STATUS "  ccache-clear      - Clear ccache")
endif()
message(STATUS "=============================================================================")

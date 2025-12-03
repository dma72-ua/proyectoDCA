# =============================================================================
# CMakeLists.txt for proyectoDCA - Juego de Plataformas
# =============================================================================
# Modern CMake configuration for multi-directory C++ project with Raylib
# Supports Linux, Windows (native and cross-compilation)
# =============================================================================

cmake_minimum_required(VERSION 3.15)

# Project definition
project(proyectoDCA
    VERSION 1.0.0
    DESCRIPTION "Juego de plataformas con Raylib"
    LANGUAGES CXX
)

# =============================================================================
# Platform Detection
# =============================================================================

if(WIN32 OR MINGW OR MSYS)
    set(PLATFORM_WINDOWS TRUE)
    message(STATUS "Target platform: Windows")
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_LINUX TRUE)
    message(STATUS "Target platform: Linux")
else()
    message(WARNING "Unsupported platform detected")
endif()

# =============================================================================
# Build Configuration
# =============================================================================

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable ccache if available (not for Windows cross-compilation)
if(NOT CMAKE_CROSSCOMPILING)
    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM)
        message(STATUS "ccache found: ${CCACHE_PROGRAM}")
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
        set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    else()
        message(STATUS "ccache not found - compilation will not be cached")
    endif()
endif()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

# Windows-specific flags
if(PLATFORM_WINDOWS)
    # Hide console window in Release builds
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        if(MINGW)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
        elseif(MSVC)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
        endif()
    endif()
    
    # Static linking for MinGW to avoid DLL dependencies
    if(MINGW)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
    endif()
endif()

# =============================================================================
# Dependencies
# =============================================================================

if(PLATFORM_WINDOWS)
    message(STATUS "Configuring for Windows build")
    
    # Windows vendor library paths
    set(VENDOR_LIB_DIR "${CMAKE_SOURCE_DIR}/vendor/lib")
    set(VENDOR_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/vendor/include")
    
    # Determine which Raylib file we need
    if(MSVC)
        set(RAYLIB_LIBRARY_FILE "raylib.lib")
        set(RAYLIB_DOWNLOAD_URL "https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_win64_msvc16.zip")
    elseif(MINGW)
        set(RAYLIB_LIBRARY_FILE "libraylib.a")
        set(RAYLIB_DOWNLOAD_URL "https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_win64_mingw-w64.zip")
    endif()
    
    # Check if Raylib exists, if not, download it
    set(RAYLIB_FOUND FALSE)
    if(EXISTS "${VENDOR_LIB_DIR}/${RAYLIB_LIBRARY_FILE}" AND EXISTS "${VENDOR_INCLUDE_DIR}/raylib.h")
        set(RAYLIB_FOUND TRUE)
        message(STATUS "Found existing Raylib in vendor directory")
    else()
        message(STATUS "Raylib not found in vendor directory")
        message(STATUS "Attempting to download Raylib automatically...")
        
        # Create vendor directories
        file(MAKE_DIRECTORY "${VENDOR_LIB_DIR}")
        file(MAKE_DIRECTORY "${VENDOR_INCLUDE_DIR}")
        
        # Download Raylib
        set(RAYLIB_ZIP "${CMAKE_BINARY_DIR}/raylib.zip")
        message(STATUS "Downloading from: ${RAYLIB_DOWNLOAD_URL}")
        
        file(DOWNLOAD 
            "${RAYLIB_DOWNLOAD_URL}"
            "${RAYLIB_ZIP}"
            SHOW_PROGRESS
            STATUS DOWNLOAD_STATUS
            TIMEOUT 300
        )
        
        list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
        list(GET DOWNLOAD_STATUS 1 ERROR_MESSAGE)
        
        if(STATUS_CODE EQUAL 0)
            message(STATUS "Download successful, extracting...")
            
            # Extract the zip file
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E tar xzf "${RAYLIB_ZIP}"
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
                RESULT_VARIABLE EXTRACT_RESULT
            )
            
            if(EXTRACT_RESULT EQUAL 0)
                # Find and copy files to vendor directory
                if(MSVC)
                    set(EXTRACT_DIR "${CMAKE_BINARY_DIR}/raylib-5.0_win64_msvc16")
                else()
                    set(EXTRACT_DIR "${CMAKE_BINARY_DIR}/raylib-5.0_win64_mingw-w64")
                endif()
                
                # Copy library files
                file(GLOB RAYLIB_LIBS "${EXTRACT_DIR}/lib/*")
                file(COPY ${RAYLIB_LIBS} DESTINATION "${VENDOR_LIB_DIR}")
                
                # Copy header files
                file(GLOB RAYLIB_HEADERS "${EXTRACT_DIR}/include/*")
                file(COPY ${RAYLIB_HEADERS} DESTINATION "${VENDOR_INCLUDE_DIR}")
                
                # Clean up
                file(REMOVE "${RAYLIB_ZIP}")
                file(REMOVE_RECURSE "${EXTRACT_DIR}")
                
                if(EXISTS "${VENDOR_LIB_DIR}/${RAYLIB_LIBRARY_FILE}")
                    set(RAYLIB_FOUND TRUE)
                    message(STATUS "✓ Raylib downloaded and installed successfully!")
                else()
                    message(FATAL_ERROR "Failed to extract Raylib library file")
                endif()
            else()
                message(FATAL_ERROR "Failed to extract Raylib: ${EXTRACT_RESULT}")
            endif()
        else()
            message(FATAL_ERROR 
                "Failed to download Raylib: ${ERROR_MESSAGE}\n"
                "Please download manually from: ${RAYLIB_DOWNLOAD_URL}\n"
                "Extract to: ${CMAKE_SOURCE_DIR}/vendor/"
            )
        endif()
    endif()
    
    # Setup Raylib library target
    if(RAYLIB_FOUND)
        add_library(raylib STATIC IMPORTED)
        set_target_properties(raylib PROPERTIES
            IMPORTED_LOCATION "${VENDOR_LIB_DIR}/${RAYLIB_LIBRARY_FILE}"
            INTERFACE_INCLUDE_DIRECTORIES "${VENDOR_INCLUDE_DIR}"
        )
        message(STATUS "Using Raylib: ${VENDOR_LIB_DIR}/${RAYLIB_LIBRARY_FILE}")
    else()
        message(FATAL_ERROR "Raylib setup failed")
    endif()

elseif(PLATFORM_LINUX)
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
endif()

# Platform-specific system dependencies
if(PLATFORM_LINUX)
    find_package(OpenGL REQUIRED)
    find_package(Threads REQUIRED)
endif()

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
list(LENGTH SOURCES SOURCE_COUNT)
message(STATUS "Found ${SOURCE_COUNT} source files")

# =============================================================================
# Executable Target
# =============================================================================

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Platform-specific linking
if(PLATFORM_WINDOWS)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        raylib
        winmm
        gdi32
        opengl32
    )
elseif(PLATFORM_LINUX)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        raylib
        OpenGL::GL
        ${CMAKE_DL_LIBS}
        m
        Threads::Threads
        rt
        X11
    )
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# Windows executable suffix
if(PLATFORM_WINDOWS)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        SUFFIX ".exe"
    )
endif()

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
# Copy Assets to Build Directory
# =============================================================================

# Determine the correct output directory based on platform and generator
if(MSVC)
    # MSVC puts executables in Debug/Release subdirectories
    set(ASSET_OUTPUT_DIR "${CMAKE_BINARY_DIR}/$<CONFIG>")
else()
    # Other generators put them directly in build directory
    set(ASSET_OUTPUT_DIR "${CMAKE_BINARY_DIR}")
endif()

# Copy assets folder to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Copying assets to build directory..."
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "${ASSET_OUTPUT_DIR}/assets"
    COMMENT "Copying assets to ${ASSET_OUTPUT_DIR}/assets"
)

# Also copy to the root build directory for convenience
if(MSVC)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "${CMAKE_BINARY_DIR}/assets"
        COMMENT "Also copying assets to ${CMAKE_BINARY_DIR}/assets"
    )
endif()

# Verify assets were copied
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Verifying assets..."
    COMMAND ${CMAKE_COMMAND} -E echo "Assets should be in: ${ASSET_OUTPUT_DIR}/assets"
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

# Custom target for ccache statistics (Linux only)
if(CCACHE_PROGRAM AND NOT CMAKE_CROSSCOMPILING)
    add_custom_target(ccache-stats
        COMMAND ${CCACHE_PROGRAM} -s
        COMMENT "Displaying ccache statistics..."
    )
    
    add_custom_target(ccache-clear
        COMMAND ${CCACHE_PROGRAM} -C
        COMMENT "Clearing ccache..."
    )
endif()

# Windows-specific: Create distribution package
if(PLATFORM_WINDOWS)
    add_custom_target(package
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/dist
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.exe ${CMAKE_BINARY_DIR}/dist/
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/dist/assets
        DEPENDS ${PROJECT_NAME}
        COMMENT "Creating Windows distribution package..."
    )
endif()

# =============================================================================
# Development Info
# =============================================================================

message(STATUS "=============================================================================")
message(STATUS "Configuration Summary:")
message(STATUS "  Project:          ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "  Platform:         ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Build type:       ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard:     C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler:         ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
if(CCACHE_PROGRAM AND NOT CMAKE_CROSSCOMPILING)
message(STATUS "  Compiler cache:   ${CCACHE_PROGRAM}")
endif()
message(STATUS "  Source files:     ${SOURCE_COUNT} files")
message(STATUS "  Install prefix:   ${CMAKE_INSTALL_PREFIX}")
if(CMAKE_CROSSCOMPILING)
message(STATUS "  Cross-compiling:  YES (${CMAKE_SYSTEM_NAME})")
endif()
message(STATUS "=============================================================================")
message(STATUS "Build targets:")
message(STATUS "  ${PROJECT_NAME}   - Build the game executable")
message(STATUS "  run               - Build and run the game")
message(STATUS "  install           - Install the game")
if(PLATFORM_WINDOWS)
message(STATUS "  package           - Create Windows distribution package")
endif()
if(CCACHE_PROGRAM AND NOT CMAKE_CROSSCOMPILING)
message(STATUS "  ccache-stats      - Show ccache statistics")
message(STATUS "  ccache-clear      - Clear ccache")
endif()
message(STATUS "=============================================================================")
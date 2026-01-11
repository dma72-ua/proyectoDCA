# Script para gestionar la dependencia de raylib
include(FetchContent)

set(FETCHCONTENT_QUIET OFF)

# RAYLIB
find_package(raylib QUIET)

if (NOT raylib_FOUND)
    message(STATUS "Raylib no encontrado, descargando desde repositorio...")
    
    # Configurar las opciones de raylib ANTES de declarar
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
    
    # Descargar raylib con progreso visible
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    
    FetchContent_MakeAvailable(raylib)
    message(STATUS "Raylib descargado y configurado correctamente")
else()
    message(STATUS "Usando Raylib instalado")
endif()

# BOOST TEST
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

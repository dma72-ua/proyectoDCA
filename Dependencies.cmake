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

# BOOST TEST - Solo en Linux
if(UNIX AND NOT APPLE)
    find_package(Boost COMPONENTS unit_test_framework)

    if (NOT Boost_FOUND)
        message(STATUS "Boost no encontrado, descargando desde repositorio...")

        FetchContent_Declare(
            Boost
            URL https://archives.boost.io/release/1.84.0/source/boost_1_84_0.tar.gz
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )

        FetchContent_GetProperties(Boost)
        if(NOT boost_POPULATED)
            message(STATUS "Descargando Boost...")
            FetchContent_Populate(Boost)
            
            # Ejecutar bootstrap para preparar Boost (solo headers, no compilar)
            message(STATUS "Preparando headers de Boost...")
            execute_process(
                COMMAND ./bootstrap.sh --with-libraries=test
                WORKING_DIRECTORY ${boost_SOURCE_DIR}
                RESULT_VARIABLE bootstrap_result
            )
            
            if(bootstrap_result)
                message(FATAL_ERROR "Bootstrap de Boost falló")
            endif()
            
            # Generar headers (no compila, solo prepara)
            execute_process(
                COMMAND ./b2 headers
                WORKING_DIRECTORY ${boost_SOURCE_DIR}
                RESULT_VARIABLE b2_result
            )
            
            if(b2_result)
                message(FATAL_ERROR "Generación de headers de Boost falló")
            endif()
            
            message(STATUS "Boost listo en: ${boost_SOURCE_DIR}")
        endif()
    endif()
elseif(WIN32)
    message(STATUS "Boost no se descarga en Windows - se asume que los tests no se compilarán")
endif()
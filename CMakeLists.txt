cmake_minimum_required(VERSION 3.15)

project(proyectoDCA
    VERSION 0.10.0
    DESCRIPTION "Juego de plataformas con Raylib"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Generar compile_commands.js para el lsp de clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor")
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")

include(Dependencies.cmake)

file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Directorio de salida unificado para MSVC
if(MSVC OR CMAKE_GENERATOR MATCHES "Visual Studio")
    set_target_properties(${PROJECT_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
            RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}"
            RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}"
    )
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
endif()

if(WIN32)
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/assets DESTINATION assets)
endif()

if(UNIX AND NOT APPLE)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/assets)
endif()

# Determinar donde poner los assets
if(MSVC)
    # MSVC pone los ejecutables en Debug/Release
    set(ASSET_OUTPUT_DIR "${CMAKE_BINARY_DIR}/$<CONFIG>")
else()
    # Otros generadores los ponen directamente en build
    set(ASSET_OUTPUT_DIR "${CMAKE_BINARY_DIR}")
endif()

# Copiamos los assets a build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMENT "Copiando assets al directorio del ejecutable"
)

# Sirve para poder hacer make run
if(UNIX AND NOT APPLE)
add_custom_target(run
    COMMAND ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Corriendo ${PROJECT_NAME}..."
)
endif()

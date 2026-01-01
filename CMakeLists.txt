cmake_minimum_required(VERSION 3.15)

project(proyectoDCA
    VERSION 0.10.0
    DESCRIPTION "Juego de plataformas con Raylib"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor")
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")

include(Dependencies.cmake)

# FIXME: Ahora recogemos automáticamente todas las sources pero luego hacerlo recursivo
file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

if(WIN32)
    install(TARGETS flappybirddca RUNTIME DESTINATION .)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/assets DESTINATION assets)
endif()

if(LINUX AND NOT APPLE)
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
        "${ASSET_OUTPUT_DIR}/assets"
    COMMENT "Copiando los assets a ${ASSET_OUTPUT_DIR}/assets"
)

# Sirve para poder hacer make run
add_custom_target(run
    COMMAND ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Corriendo ${PROJECT_NAME}..."
)
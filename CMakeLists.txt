cmake_minimum_required(VERSION 3.15)

project(proyectoDCA
    VERSION 1.0.0
    DESCRIPTION "Juego de plataformas con Raylib"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generar compile_commands.js para el lsp de clangd

set(VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor")
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")

include(Dependencies.cmake)

file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)
list(FILTER SOURCES EXCLUDE REGEX ".*src/core/main.cpp$")

add_library(core_lib STATIC ${SOURCES} ${HEADERS})
target_include_directories(core_lib PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(core_lib PUBLIC raylib)

add_executable(${PROJECT_NAME} src/core/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE core_lib)

source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${SOURCES})

enable_testing()
add_subdirectory(tests)

set(CPACK_PACKAGE_NAME "proyectoDCA")
set(CPACK_PACKAGE_VENDOR "Equipo de DCA: Enrique, Adnan, Salma y David")
set(CPACK_PACKAGE_VERSION "1.0.0")

if(WIN32)
    install(TARGETS proyectoDCA RUNTIME DESTINATION .)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/assets DESTINATION .)
    set(CPACK_GENERATOR "NSIS")
    
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "proyectoDCA")
    set(CPACK_NSIS_DISPLAY_NAME "proyectoDCA")
    set(CPACK_NSIS_PACKAGE_NAME "proyectoDCA")
    
    set(CPACK_NSIS_INSTALLED_ICON_NAME "proyectoDCA.exe")
    set(CPACK_NSIS_MENU_LINKS "proyectoDCA.exe" "proyectoDCA")

endif()

if(UNIX AND NOT APPLE)
    install(TARGETS proyectoDCA DESTINATION bin)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/assets 
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})

    # Para DEB
    set(CPACK_GENERATOR "DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Equipo de DCA: Enrique, Adnan, Salma y David")
    set(CPACK_DEBIAN_PACKAGE_SECTION "games")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libraylib (>=5.0)")

    # Para RPM
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    set(CPACK_RPM_PACKAGE_GROUP "Amusements/Games")
    set(CPACK_RPM_PACKAGE_REQUIRES "raylib >= 5.0")

    set(CPACK_PACKAGE_DESCRIPTION "Juego 2D de plataformas")
endif()

include(CPack)

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
    set(ASSET_OUTPUT_DIR "${CMAKE_BINARY_DIR}/$<CONFIG>") # MSVC pone los ejecutables en Debug/Release
else()
    set(ASSET_OUTPUT_DIR "${CMAKE_BINARY_DIR}") # Otros generadores los ponen directamente en build
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

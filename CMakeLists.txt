cmake_minimum_required(VERSION 3.20)

project(AP4A_IoT_AirQuality
    VERSION 0.1.0
    DESCRIPTION "Simulateur d'Ecosysteme IoT & Qualite de l'Air"
    LANGUAGES CXX
)

# ------------------------------------------------------------
# Standard C++
# ------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------
# Options de compilation (MSVC vs GCC/Clang)
# ------------------------------------------------------------
if (MSVC)
    add_compile_options(/W4 /permissive- /Zc:__cplusplus)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ------------------------------------------------------------
# Generation de Config.h (chemins absolus injectes proprement)
# ------------------------------------------------------------
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/core/Config.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/core/Config.h"
    @ONLY
)

# ------------------------------------------------------------
# Cible core (console)
# ------------------------------------------------------------
file(GLOB CORE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp"
)

add_executable(simulator_core
    ${CORE_SOURCES}
    src/main_core.cpp
)

target_include_directories(simulator_core PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# ------------------------------------------------------------
# Qt6 (Phase 4) - desactive tant que non trouve
# ------------------------------------------------------------
find_package(Qt6 COMPONENTS Widgets Charts QUIET)

if (Qt6_FOUND)
    message(STATUS "Qt6 trouve -> cible GUI activee (Phase 4)")
else()
    message(STATUS "Qt6 non trouve -> mode console uniquement")
endif()
cmake_minimum_required(VERSION 3.20)

project(AP4A_IoT_AirQuality
    VERSION 0.2.0
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
# Options compilateur
# ------------------------------------------------------------
if (MSVC)
    add_compile_options(/W4 /permissive- /Zc:__cplusplus)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ------------------------------------------------------------
# Config.h
# ------------------------------------------------------------
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/core/Config.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/core/Config.h"
    @ONLY
)

# ------------------------------------------------------------
# Recherche de Qt6 (hint pour Windows)
# ------------------------------------------------------------
if (WIN32 AND NOT CMAKE_PREFIX_PATH)
    foreach(_qt_candidate
            "C:/Qt/6.11.2/msvc2022_64"
            "C:/Qt/6.7.0/msvc2019_64"
            "$ENV{QTDIR}")
        if (EXISTS "${_qt_candidate}/lib/cmake/Qt6")
            set(CMAKE_PREFIX_PATH "${_qt_candidate}" CACHE PATH "Qt kit path")
            message(STATUS "Qt6 candidate trouve : ${_qt_candidate}")
            break()
        endif()
    endforeach()
endif()

# ------------------------------------------------------------
# Sources core (partagees console + GUI)
# ------------------------------------------------------------
file(GLOB CORE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp"
)

# ------------------------------------------------------------
# Cible console : simulator_core
# ------------------------------------------------------------
add_executable(simulator_core
    ${CORE_SOURCES}
    src/main_core.cpp
)
target_include_directories(simulator_core PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# ------------------------------------------------------------
# Cible GUI Qt : simulator_gui (optionnelle)
# ------------------------------------------------------------
find_package(Qt6 COMPONENTS Widgets Charts QUIET)

if (Qt6_FOUND)
    message(STATUS "Qt6 trouve -> cible simulator_gui activee")

    set(CMAKE_AUTOMOC ON)

    file(GLOB UI_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/*.cpp"
    )
    file(GLOB UI_HEADERS CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/ui/*.h"
    )

    add_executable(simulator_gui
        ${CORE_SOURCES}
        ${UI_SOURCES}
        ${UI_HEADERS}
        src/main_qt.cpp
    )
    target_include_directories(simulator_gui PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )
    target_link_libraries(simulator_gui PRIVATE
        Qt6::Widgets
        Qt6::Charts
    )
else()
    message(STATUS "Qt6 non trouve -> cible GUI desactivee")
endif()
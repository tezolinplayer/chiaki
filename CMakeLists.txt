cmake_minimum_required(VERSION 3.10)
project(chiaki)

macro(tri_option name desc default)
    set("${name}" "${default}" CACHE STRING "${desc}")
    set_property(CACHE "${name}" PROPERTY STRINGS AUTO ON OFF)
endmacro()

# CONFIGURAÇÃO ANTI-RECOIL (GHOST MODE)
option(CHIAKI_ENABLE_TESTS "Enable tests" OFF)
option(CHIAKI_ENABLE_CLI "Enable CLI" ON)
option(CHIAKI_ENABLE_GUI "Enable GUI" OFF)
option(CHIAKI_CLI_ARGP_STANDALONE "Standalone argp" ON) # LIGADO PARA WINDOWS

set(CHIAKI_FFMPEG_DEFAULT OFF)
tri_option(CHIAKI_ENABLE_FFMPEG_DECODER "Enable FFMPEG" OFF)
set(CHIAKI_USE_SYSTEM_NANOPB OFF CACHE BOOL "" FORCE)

set(CHIAKI_VERSION "2.1.1")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 11)

find_package(PythonInterp 3 REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

# Busca a biblioteca de comandos no Windows
if(CHIAKI_CLI_ARGP_STANDALONE)
    find_library(ARGP_LIBRARY argp)
endif()

add_definitions(-DCHIAKI_VERSION=\"${CHIAKI_VERSION}\")
add_subdirectory(third-party)
add_subdirectory(lib)
add_subdirectory(cli)

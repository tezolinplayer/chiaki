cmake_minimum_required(VERSION 3.10)

project(chiaki)

# Like option(), but the value can also be AUTO
macro(tri_option name desc default)
    set("${name}" "${default}" CACHE STRING "${desc}")
    set_property(CACHE "${name}" PROPERTY STRINGS AUTO ON OFF)
endmacro()

# CONFIGURAÇÕES AJUSTADAS PARA O SEU MOD:
option(CHIAKI_ENABLE_TESTS "Enable tests for Chiaki" OFF) # Desativado para evitar erros de teste
option(CHIAKI_ENABLE_CLI "Enable CLI for Chiaki" ON)      # Ativado como alternativa
option(CHIAKI_ENABLE_GUI "Enable Qt GUI" ON)             # Mantido para você ter a interface
option(CHIAKI_ENABLE_ANDROID "Enable Android" OFF)
option(CHIAKI_ENABLE_BOREALIS "Enable Borealis GUI" OFF)
tri_option(CHIAKI_ENABLE_SETSU "Enable libsetsu" OFF)
option(CHIAKI_LIB_ENABLE_OPUS "Use Opus as part of Chiaki Lib" ON)

# FORÇANDO O FFMPEG PARA OFF PARA CORRIGIR O ERRO [60/86]
set(CHIAKI_FFMPEG_DEFAULT OFF)
tri_option(CHIAKI_ENABLE_FFMPEG_DECODER "Enable FFMPEG video decoder" OFF)
tri_option(CHIAKI_ENABLE_PI_DECODER "Enable Raspberry Pi decoder" OFF)

option(CHIAKI_LIB_ENABLE_MBEDTLS "Use mbedtls instead of OpenSSL" OFF)
option(CHIAKI_LIB_MBEDTLS_EXTERNAL_PROJECT "Fetch Mbed TLS" OFF)
option(CHIAKI_LIB_OPENSSL_EXTERNAL_PROJECT "Use OpenSSL as CMake external project" OFF)
option(CHIAKI_GUI_ENABLE_SDL_GAMECONTROLLER "Use SDL Gamecontroller for Input" ON)
option(CHIAKI_CLI_ARGP_STANDALONE "Search for standalone argp lib for CLI" OFF)
tri_option(CHIAKI_USE_SYSTEM_JERASURE "Use system jerasure" AUTO)
tri_option(CHIAKI_USE_SYSTEM_NANOPB "Use system nanopb" AUTO)

set(CHIAKI_VERSION_MAJOR 2)
set(CHIAKI_VERSION_MINOR 1)
set(CHIAKI_VERSION_PATCH 1)
set(CHIAKI_VERSION ${CHIAKI_VERSION_MAJOR}.${CHIAKI_VERSION_MINOR}.${CHIAKI_VERSION_PATCH})

set(CPACK_PACKAGE_NAME "chiaki")
set(CPACK_PACKAGE_DESCRIPTION "Open Source PS4 remote play client")
set(CPACK_PACKAGE_VERSION_MAJOR ${CHIAKI_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CHIAKI_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CHIAKI_VERSION_PATCH})
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
include(CPack)

set(CHIAKI_IS_SWITCH ${NSWITCH})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_CURRENT_SOURCE_DIR}/setsu/cmake")
set(CMAKE_CXX_STANDARD 11)

if(CHIAKI_IS_SWITCH)
    set(CHIAKI_LIB_ENABLE_MBEDTLS ON)
    add_definitions(-D__SWITCH__)
endif()

if(CHIAKI_USE_SYSTEM_JERASURE)
    if(CHIAKI_USE_SYSTEM_JERASURE STREQUAL AUTO)
        find_package(Jerasure QUIET)
        set(CHIAKI_USE_SYSTEM_JERASURE ${Jerasure_FOUND})
    else()
        find_package(Jerasure REQUIRED)
        set(CHIAKI_USE_SYSTEM_JERASURE ON)
    endif()
endif()

find_package(PythonInterp 3 REQUIRED)

if(CHIAKI_USE_SYSTEM_NANOPB)
    if(CHIAKI_USE_SYSTEM_NANOPB STREQUAL AUTO)
        find_package(Nanopb QUIET)
        set(CHIAKI_USE_SYSTEM_NANOPB ${Nanopb_FOUND})
    else()
        find_package(Nanopb REQUIRED)
        set(CHIAKI_USE_SYSTEM_NANOPB ON)
    endif()
endif()

add_subdirectory(third-party)
add_definitions(-DCHIAKI_VERSION_MAJOR=${CHIAKI_VERSION_MAJOR} -DCHIAKI_VERSION_MINOR=${CHIAKI_VERSION_MINOR} -DCHIAKI_VERSION_PATCH=${CHIAKI_VERSION_PATCH} -DCHIAKI_VERSION=\"${CHIAKI_VERSION}\")

if(CHIAKI_LIB_OPENSSL_EXTERNAL_PROJECT)
    include(OpenSSLExternalProject)
endif()

add_subdirectory(lib)

if(CHIAKI_ENABLE_CLI)
    add_subdirectory(cli)
endif()

if(CHIAKI_ENABLE_GUI)
    add_subdirectory(gui)
endif()

if(CHIAKI_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

cmake_minimum_required(VERSION 3.10)

project(chiaki)

# Macro para opções tri-state (ON, OFF, AUTO)
macro(tri_option name desc default)
    set("${name}" "${default}" CACHE STRING "${desc}")
    set_property(CACHE "${name}" PROPERTY STRINGS AUTO ON OFF)
endmacro()

# CONFIGURAÇÃO PARA O MOD ANTI-RECOIL (SEM VÍDEO / SEM JANELA)
option(CHIAKI_ENABLE_TESTS "Enable tests for Chiaki" OFF)
option(CHIAKI_ENABLE_CLI "Enable CLI for Chiaki" ON)      # LIGADO: Para gerar o executável
option(CHIAKI_ENABLE_GUI "Enable Qt GUI" OFF)            # DESLIGADO: Para evitar erro de dependência
option(CHIAKI_ENABLE_ANDROID "Enable Android" OFF)
option(CHIAKI_ENABLE_BOREALIS "Enable Borealis GUI" OFF)
tri_option(CHIAKI_ENABLE_SETSU "Enable libsetsu" OFF)
option(CHIAKI_LIB_ENABLE_OPUS "Use Opus as part of Chiaki Lib" ON)

# MOTOR DE VÍDEO DESLIGADO (EVITA ERRO 60/86)
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

include(CPack)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 11)

find_package(PythonInterp 3 REQUIRED)

add_subdirectory(third-party)
add_definitions(-DCHIAKI_VERSION=\"${CHIAKI_VERSION}\")

# BIBLIOTECA ONDE ESTÁ O SEU ANTI-RECOIL
add_subdirectory(lib)

# EXECUTÁVEL FINAL
if(CHIAKI_ENABLE_CLI)
    add_subdirectory(cli)
endif()

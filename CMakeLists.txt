cmake_minimum_required(VERSION 3.25)

project(GfxVTable
    VERSION 0.1.0
    LANGUAGES CXX
)

# ----------------------------------------------------------------------
# Compiler flags
# ----------------------------------------------------------------------

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(GFXVTABLE_NO_EXCEPTIONS -fno-exceptions -fno-unwind-tables)
    set(GFXVTABLE_NO_RTTI -fno-rtti)
    set(GFXVTABLE_WARNING_OPTIONS -Wall -Wextra)
elseif(MSVC OR CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
    set(GFXVTABLE_NO_EXCEPTIONS /EHs- /EHc- /D_HAS_EXCEPTIONS=0)
    set(GFXVTABLE_NO_RTTI /GR-)
    set(GFXVTABLE_WARNING_OPTIONS /W4 /utf-8)
endif()

# ----------------------------------------------------------------------
# Library
# ----------------------------------------------------------------------

add_library(GfxVTable INTERFACE)
add_library(GfxVTable::GfxVTable ALIAS GfxVTable)

target_compile_features(GfxVTable INTERFACE cxx_std_23)

target_include_directories(GfxVTable INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_compile_options(GfxVTable INTERFACE
    ${GFXVTABLE_NO_EXCEPTIONS}
    ${GFXVTABLE_NO_RTTI}
    ${GFXVTABLE_WARNING_OPTIONS}
)

if(GFXVTABLE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# CMake configuration for 110-finalproject
# Self-contained build -- run: cmake -B build -S . from this directory

cmake_minimum_required(VERSION 3.20...4.0)

project(110FinalProject VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compiler warnings
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

# ROOT_DIR
add_compile_definitions(ROOT_DIR="${CMAKE_SOURCE_DIR}/..")

# Find SDL3 package
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)

# Fetch dependencies using FetchContent
include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# Fetch Lua 5.4 source and build it as a static library
FetchContent_Declare(
    lua
    GIT_REPOSITORY https://github.com/lua/lua.git
    GIT_TAG v5.4.7
)

if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

FetchContent_GetProperties(lua)
if(NOT lua_POPULATED)
    FetchContent_Populate(lua)
endif()

# Build Lua as a static C library from source
file(GLOB LUA_SOURCES "${lua_SOURCE_DIR}/*.c")
# Remove lua.c (standalone interpreter) and luac.c (compiler) from the sources
list(FILTER LUA_SOURCES EXCLUDE REGEX "lua\\.c$")
list(FILTER LUA_SOURCES EXCLUDE REGEX "luac\\.c$")

add_library(lua_static STATIC ${LUA_SOURCES})
target_include_directories(lua_static PUBLIC ${lua_SOURCE_DIR})
# Compile as C, not C++
set_target_properties(lua_static PROPERTIES LINKER_LANGUAGE C)

# Enable testing
enable_testing()

# ============================================================================
# Engine and Game
# ============================================================================
add_subdirectory(engine/tests)
add_subdirectory(game)
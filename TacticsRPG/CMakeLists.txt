cmake_minimum_required(VERSION 3.10.0)

project(TacticsRPG VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)
FetchContent_MakeAvailable(raylib)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "classes/*.cpp")

add_executable(TacticsRPG
    main.cpp
    ${SOURCES}
)
target_link_libraries(TacticsRPG PRIVATE raylib)
target_include_directories(TacticsRPG PRIVATE classes)


// Compiler warnings
if(MSVC)
    target_compile_options(TacticsRPG PRIVATE /W4 /WX)
else()
    target_compile_options(TacticsRPG PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
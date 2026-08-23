cmake_minimum_required(VERSION 3.20)

project(OrbitalRescueStarter LANGUAGES C CXX)


# ============================================================
# Language standards
# ============================================================

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# ============================================================
# Raylib
# ============================================================

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

add_subdirectory(raylib-5.5 EXCLUDE_FROM_ALL)


# ============================================================
# Main Orbital Rescue executable
# ============================================================

add_executable(
        orbital_rescue

        src/main.cpp
        src/Spacecraft.cpp
        src/Simulation.cpp
        src/MissionConfig.cpp
        src/wrapAngle.cpp
        src/AP.cpp
        src/Input.cpp
        src/UI.cpp

        include/MissionConfig.hpp
        include/Buttons.hpp
        include/AP.hpp
        include/WrapAngle.hpp
)


target_include_directories(
        orbital_rescue
        PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
)


target_link_libraries(
        orbital_rescue
        PRIVATE
        raylib
)


# ============================================================
# Compiler warnings
# ============================================================

set_target_properties(
        orbital_rescue
        PROPERTIES
        COMPILE_WARNING_AS_ERROR ON
)


if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
            orbital_rescue
            PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wconversion
    )
endif()


# ============================================================
# Clang-Tidy
# ============================================================

find_program(CLANG_TIDY_EXE NAMES clang-tidy)

if(CLANG_TIDY_EXE)
    set_target_properties(
            orbital_rescue
            PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
    )
endif()


# ============================================================
# GoogleTest
# Local copy:
#
# project/
# ├── CMakeLists.txt
# ├── googletest/
# ├── raylib-5.5/
# ├── src/
# ├── include/
# └── tests/
# ============================================================

add_subdirectory(googletest EXCLUDE_FROM_ALL)

enable_testing()


# ============================================================
# Unit tests
# ============================================================

add_executable(
        orbital_tests

        tests/Vec2_tests.cpp
        src/WrapAngle.cpp
        tests/WrapAngle_tests.cpp
)


target_include_directories(
        orbital_tests
        PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
)


target_link_libraries(
        orbital_tests
        PRIVATE
        GTest::gtest_main
)


# ============================================================
# Automatically discover GoogleTest tests
# ============================================================

include(GoogleTest)

gtest_discover_tests(orbital_tests)
cmake_minimum_required(VERSION 3.20)

project(OrbitalRescueStarter LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

add_subdirectory(raylib-5.5 EXCLUDE_FROM_ALL)

add_executable(orbital_rescue
    src/main.cpp
    src/Spacecraft.cpp
    src/Simulation.cpp
        include/MissionConfig.h
        src/MissionConfig.cpp
        include/buttons.h
        src/buttons.cpp
        include/AP.h
        include/wrapAngle.h
        src/wrapAngle.cpp
src/AP.cpp

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

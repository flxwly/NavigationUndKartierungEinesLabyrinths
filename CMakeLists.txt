cmake_minimum_required(VERSION 3.20)
project(NavigationUndKartierungEinesLabyrinths)

set(CMAKE_CXX_STANDARD 17)

## If you want to link SFML statically
# set(SFML_STATIC_LIBRARIES TRUE)

## In most cases better set in the CMake cache
set(SFML_DIR "C:\\GCC_Compiler\\SFML-2.5.1x32\\lib\\cmake\\SFML")

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

add_executable(NavigationUndKartierungEinesLabyrinths
        main.cpp
        Robot/Cell.cpp Robot/Cell.hpp
        Robot/LabyrinthMap.cpp Robot/LabyrinthMap.hpp
        Robot/Robot.cpp Robot/Robot.hpp
        Robot/LaserRangeSensor.cpp Robot/LaserRangeSensor.hpp
        Wall.cpp Wall.hpp
        Map.cpp Map.hpp)

target_link_libraries(NavigationUndKartierungEinesLabyrinths sfml-graphics)
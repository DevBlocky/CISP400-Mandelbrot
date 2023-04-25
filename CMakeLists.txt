cmake_minimum_required(VERSION 3.25)
project(mandelbrot)
set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)

add_executable(mandelbrot src/main.cpp src/ComplexPlane.cpp src/ComplexPlane.h)

target_link_libraries(mandelbrot sfml-system sfml-window sfml-graphics)

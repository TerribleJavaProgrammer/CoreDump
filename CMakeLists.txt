PROJECT_NAME = "core_dump"
cmake_minimum_required(VERSION 3.4)
project(${PROJECT_NAME})
add_subdirectory(pybind11)
pybind11_add_module(${PROJECT_NAME} api/src/main.cpp)

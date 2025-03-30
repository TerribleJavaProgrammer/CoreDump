cmake_minimum_required(VERSION 3.28)
project(core_dump)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Collect source files
file(GLOB_RECURSE API_SOURCES "./api/src/*.cpp")

# Create the console app executable
add_executable(${PROJECT_NAME} ${API_SOURCES})

# Include the header directory
target_include_directories(${PROJECT_NAME} PRIVATE "./api/include")
target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::module)

# Add pybind11

# Create a Python module with a different target name
add_subdirectory(pybind11)
set(PYTHON_MODULE_NAME "${PROJECT_NAME}_py")
pybind11_add_module(${PYTHON_MODULE_NAME} ${API_SOURCES})
target_include_directories(${PYTHON_MODULE_NAME} PRIVATE "./api/include")

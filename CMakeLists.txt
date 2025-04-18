cmake_minimum_required(VERSION 3.28)
project(core_dump)

include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Use IPO if it's supported
if( IPO_SUPPORTED )
	message(STATUS "IPO / LTO enabled by default")
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
	message(STATUS "IPO / LTO not supported: <${IPO_ERROR}>")
endif()

# Collect source files
file(GLOB_RECURSE API_SOURCES "api/src/*.cpp")

find_package(Python REQUIRED COMPONENTS Interpreter Development)

# Build the pybind library submodule
add_subdirectory(pybind11)

# Create the console app executable
#add_executable(${PROJECT_NAME} ${API_SOURCES})

# Create a Python module with a different target name
set(PYTHON_MODULE_NAME "${PROJECT_NAME}_py")
pybind11_add_module(${PYTHON_MODULE_NAME} ${API_SOURCES})

#target_include_directories(${PROJECT_NAME} PRIVATE "api/include")
#target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::module)
#target_link_libraries(${PROJECT_NAME} PRIVATE Python::Python)

target_include_directories(${PYTHON_MODULE_NAME} PRIVATE "api/include")
target_link_libraries(${PYTHON_MODULE_NAME} PRIVATE pybind11::module)
target_link_libraries(${PYTHON_MODULE_NAME} PRIVATE Python::Python)


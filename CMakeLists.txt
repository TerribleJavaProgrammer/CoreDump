cmake_minimum_required(VERSION 3.28)
project(core_dump)
set(PYBIND_INCLUDE_DIR "./env/lib/python3.12/site-packages/pybind11/include;/usr/include/python3.12")
# Create the console app exectuable
add_executable(${PROJECT_NAME} ./api/src/main.cpp)

file(GLOB_RECURSE API_SOURCES "./api/src/*.cpp")
file(GLOB_RECURSE API_HEADERS "./api/include/*.h")
set(SOURCES ${API_SOURCES} ${API_HEADERS}) 

target_include_directories(${PROJECT_NAME} PRIVATE ./api/include PRIVATE ${PYBIND_INCLUDE_DIR})

target_link_directories(${PROJECT_NAME} PRIVATE ./api/src)

# Init pybind 
include_directories(${PYBIND_INCLUDE_DIR})
add_subdirectory(pybind11)
pybind11_add_module("${PROJECT_NAME}_pymodule" ${SOURCES})

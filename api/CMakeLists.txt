# Set the source directory
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Set the api project name
set(API_NAME "core_dump_api")

# Include pybind dirs
include_directories("/home/gili/Documents/CoreDump/env/lib/python3.12/site-packages/pybind11/include;/usr/include/python3.12")

# Find all .cpp and .h files in the api directory
file(GLOB_RECURSE API_SOURCES "${SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE API_HEADERS "${SOURCE_DIR}/*.h")

#foreach(file IN LISTS API_SOURCES)
#	message(STATUS "cpp file: ${file}")
#endforeach()

#foreach(file IN LISTS API_HEADERS)
#	message(STATUS "header file: ${file}")
#endforeach()

# Add the executable or library
add_library(${API_NAME} STATIC ${API_SOURCES} ${API_HEADERS})

# Include directories
target_include_directories(${API_NAME} PUBLIC ${SOURCE_DIR})

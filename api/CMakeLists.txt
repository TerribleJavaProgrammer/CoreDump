# Set the source directory
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Find all .cpp and .h files in the api directory
file(GLOB_RECURSE API_SOURCES "${SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE API_HEADERS "${SOURCE_DIR}/*.h")

# Add the executable or library
add_library(${PROJECT_NAME} STATIC ${API_SOURCES} ${API_HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC ${SOURCE_DIR})
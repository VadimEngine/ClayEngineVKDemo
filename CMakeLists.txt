cmake_minimum_required(VERSION 3.10)

project(ClayEngineVKDemo)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all source files
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
# Add the executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Define the DEMO_RESOURCE_PATH macro
target_compile_definitions(${PROJECT_NAME} PRIVATE DEMO_RESOURCE_PATH="${CMAKE_SOURCE_DIR}/res")

# Set Clay to Desktop mode
set(CLAY_PLATFORM_DESKTOP ON CACHE BOOL "Set Platform to Desktop" FORCE)

# Add ClayEngine as a subdirectory and link to it
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/thirdparty/ClayEngineVK 
    ${CMAKE_BINARY_DIR}/thirdparty/ClayEngineVK
)
target_link_libraries(${PROJECT_NAME} PRIVATE ClayEngineVK)

# Include project-specific directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

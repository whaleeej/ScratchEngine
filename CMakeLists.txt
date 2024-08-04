cmake_minimum_required(VERSION 3.30.0)

file(GLOB_RECURSE RUNTIME_FILES Runtime/*.h Runtime/*.hpp Runtime/*.cpp)

project(ScratchEngine LANGUAGES CXX)
add_executable(ScratchEngine WIN32 ${RUNTIME_FILES})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ScratchEngine)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RUNTIME_FILES})
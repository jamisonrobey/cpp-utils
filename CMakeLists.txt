cmake_minimum_required(VERSION 4.0)
project (jam_utils LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(jam_utils INTERFACE)

target_include_directories(jam_utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(dummy)


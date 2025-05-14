cmake_minimum_required(VERSION 4.0)
project (jamutils LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(jamutils INTERFACE)
target_include_directories(jamutils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)




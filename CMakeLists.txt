cmake_minimum_required(VERSION 3.6)
project(cpp_json)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Json.cpp Json.h JsonParser.cpp JsonParser.h)
add_executable(cpp_json ${SOURCE_FILES})
project(FixedPointLib)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra  -Wold-style-cast -g  ${CMAKE_CXX_FLAGS}") #-Weffc++ -Werror
set(SOURCE_FILES main.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES} "fixed.h")
add_executable(Interactive MathParser.cpp)


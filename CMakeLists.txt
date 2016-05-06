project(FixedPointLib)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra  -Wold-style-cast -g  ${CMAKE_CXX_FLAGS}") #-Weffc++ -Werror
add_executable(${PROJECT_NAME} ${SRC_LIST} "fixed.h")


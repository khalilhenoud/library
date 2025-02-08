cmake_minimum_required(VERSION 3.22)

# set the project name
project(library VERSION 1.0)

# add the executable
add_library(${PROJECT_NAME} SHARED
      ./source/streams/binary_stream.cpp
			./source/type_registry/type_registry.c
			./include/library/internal/module.h)

target_include_directories(${PROJECT_NAME} PUBLIC 
			"${PROJECT_SOURCE_DIR}/include")
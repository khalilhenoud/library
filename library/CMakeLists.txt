cmake_minimum_required(VERSION 3.22)

# set the project name
project(library VERSION 1.0)

# add the executable
add_library(${PROJECT_NAME} SHARED
      ./source/filesystem/io.c
      ./source/filesystem/filesystem.c
      ./source/streams/binary_stream.c
      ./source/type_registry/default_registry.c
			./source/type_registry/type_registry.c
			./include/library/internal/module.h)

target_include_directories(${PROJECT_NAME} PUBLIC 
			"${PROJECT_SOURCE_DIR}/include")
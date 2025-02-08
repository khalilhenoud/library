cmake_minimum_required(VERSION 3.22)

# all libraries built by this project are copied into the output directory.
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

project(library_test VERSION 1.0)

add_subdirectory(../library library)

# specify the cpp standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(${PROJECT_NAME} 
        ./source/classroom.c
				./source/main.cpp
        ./source/cvector_test.cpp
        ./source/clist_test.cpp
        ./source/hash_test.cpp
        ./source/chashmap_test.cpp
        ./source/binary_stream_test.cpp
        ./source/type_registry_test.cpp
				)

target_link_libraries(${PROJECT_NAME}
        PUBLIC library)

target_include_directories(${PROJECT_NAME} PUBLIC 
							"${PROJECT_BINARY_DIR}"
							"${PROJECT_SOURCE_DIR}/include"
							)
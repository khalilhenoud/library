cmake_minimum_required(VERSION 3.22)

project(library_package)

if(ENGINE_BUILD)
add_subdirectory(library)
else()
add_subdirectory(library_test)
endif()
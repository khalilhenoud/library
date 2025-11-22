cmake_minimum_required(VERSION 3.22)

project(library_package)

if (HAS_STANDALONE_PREDECESSOR)
add_subdirectory(library)
else()
add_subdirectory(library_test)
endif()
cmake_minimum_required(VERSION 3.22)

# set the project name
project(library VERSION 1.0)

# add the executable
add_library(${PROJECT_NAME} SHARED
      ./source/allocator/allocator.c
      ./source/filesystem/io.c
      ./source/filesystem/filesystem.c
      ./source/framerate_controller/framerate_controller.c
      ./source/os/os.c
      ./source/memory/memory.c
      ./source/streams/binary_stream.c
      ./source/type_registry/default_registry.c
			./source/type_registry/type_registry.c
			./include/library/internal/module.h)

# winmm is required for timeGetTime(), used by frameratecontroller. library is
# currently tied to windows, in the future this will change and this might need
# revisiting.
target_link_libraries(${PROJECT_NAME} PUBLIC winmm)

target_include_directories(${PROJECT_NAME} PUBLIC 
			"${PROJECT_SOURCE_DIR}/include")
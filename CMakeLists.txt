# cmake CMakeLists.txt -B build -S ${PWD}
# make -C build -j4

cmake_minimum_required(VERSION 3.22)
project(ruthenium LANGUAGES CXX VERSION 1.0.0.0)
set(output_name "${PROJECT_NAME}-${PROJECT_VERSION}")
set(libs
    libglfw3.a
    GL
    libGLEW.a
    X11
    pthread
    Xrandr
    Xi
    dl
)

set(libdirs
    lib
)

set(defs
    GLEW_STATIC
)

set(include
    include
    vendor
)

set(source
    src/main.cpp
    src/window.cpp
    src/core.cpp
    src/clock.cpp

    src/subsys/log_manager.cpp
    #src/memory/stack_allocator.cpp
)

set(options
    -Wall
    -Wextra
    -Wpedantic
    -std=c++17
    -g3
    -ggdb3
    -fmax-errors=10
)

add_executable(${output_name})
target_link_directories(${output_name} PRIVATE ${libdirs})
target_link_libraries(${output_name} PRIVATE ${libs})
target_compile_definitions(${output_name} PRIVATE ${defs})
target_include_directories(${output_name} PRIVATE ${include})
target_sources(${output_name} PRIVATE ${source})
target_compile_options(${output_name} PRIVATE ${options})
set_target_properties(${output_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "bin/Debug")

cmake_minimum_required(VERSION 3.10)

project(pax_linux_x11 C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(ENV{PAX_CMAKE_STANDALONE} True)
set(PAX_COMPILE_MCR 0)

add_executable(pax_linux_x11 src/main.c)

add_subdirectory(components/pax-gfx)

target_include_directories(pax_linux_x11 PUBLIC components/pax-gfx/src)
target_link_libraries(pax_linux_x11 pthread X11 pax_graphics)

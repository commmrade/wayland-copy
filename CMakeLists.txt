cmake_minimum_required(VERSION 3.16)
project(proj LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Wayland libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-protocols)

# Include directories
include_directories(${WAYLAND_INCLUDE_DIRS})
link_directories(${WAYLAND_LIBRARY_DIRS})
add_definitions(${WAYLAND_CFLAGS_OTHER})

# Source files
add_executable(proj main.cpp)

# Link libraries
target_link_libraries(proj PRIVATE ${WAYLAND_LIBRARIES})

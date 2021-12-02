cmake_minimum_required(VERSION 3.13)

project(beluga)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS}")

find_package(fmt CONFIG REQUIRED)

add_executable(beluga
    beluga.m.cpp
    noise_maker.cpp
)

target_link_libraries(beluga PRIVATE
    fmt::fmt
)

target_include_directories(beluga PRIVATE .)
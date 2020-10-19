cmake_minimum_required(VERSION 3.17)
project(rhope C)

set(CMAKE_C_STANDARD 99)

include_directories(hdr)
file(GLOB SRCFILES src/*.c)

add_executable(rhope ${SRCFILES})
target_link_libraries(rhope uv telnet json-c)
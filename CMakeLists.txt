cmake_minimum_required(VERSION 3.17)
project(rhope C)

set(CMAKE_C_STANDARD 99)

include_directories(hdr)
file(GLOB SRCFILES src/*.c)

add_executable(rhope ${SRCFILES})
set_target_properties(rhope PROPERTIES ENABLE_EXPORTS TRUE)
target_link_libraries(rhope uv json-c)

add_library(mudtelnet SHARED psrc/telnet.c)
target_link_libraries(mudtelnet telnet)

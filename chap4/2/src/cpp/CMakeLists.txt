cmake_minimum_required(VERSION 3.15)
project(transparent_bridge)

set(CMAKE_CXX_STANDARD 17)

add_executable(bridge main.cpp SendTable.cpp SendTable.h)
target_link_libraries(bridge pcap)
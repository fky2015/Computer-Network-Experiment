cmake_minimum_required(VERSION 3.15)
project(chap-vertify)

set(CMAKE_CXX_STANDARD 14)

add_executable(chap main.cpp md5.cpp md5.h randombyte.cpp randombyte.h)
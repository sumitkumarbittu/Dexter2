cmake_minimum_required(VERSION 3.10)
project(HelloForm)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Wt REQUIRED)

include_directories("/usr/local/include")
link_directories("/usr/local/lib")

add_executable(hello_form main.cpp)
target_link_libraries(hello_form PRIVATE wt wthttp)

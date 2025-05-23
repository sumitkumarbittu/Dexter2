cmake_minimum_required(VERSION 3.5)
project(WebCompress)

find_package(Wt REQUIRED COMPONENTS Http)
find_package(SQLite3 REQUIRED)

include_directories(
    ${Wt_INCLUDE_DIRS}
    ${SQLite3_INCLUDE_DIRS}
)

add_executable(web_compress
    web_compress.cpp
    database.cpp
    lzw_compressor.cpp
)

target_link_libraries(web_compress
    ${Wt_LIBRARIES}
    ${SQLite3_LIBRARIES}
)
cmake_minimum_required(VERSION 3.14)
project(sqlite)

set(CMAKE_CXX_STANDARD 17)

set(FLAGS "-Wall -undefined dynamic_lookup")
set(CMAKE_CXX_FLAGS "${FLAGS}")
set(EXECUTABLE_OUTPUT_PATH "../lib")

include_directories(../lib/**.h)
include_directories(/usr/local/opt/dart/libexec/include)
include_directories(/third_party/**.c)
include_directories(/third_party/**.h)
add_compile_definitions(DART_SHARED_LIB=1)

add_library(sqlite SHARED
        src/dart.cpp
        src/dart.h
        third_party/sqlite-amalgamation-3290000/sqlite3.c
        third_party/sqlite-amalgamation-3290000/sqlite3.h)

set_target_properties(sqlite
        PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "../../lib/src/")
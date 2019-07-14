cmake_minimum_required(VERSION 3.14)
project(sqlite)

set(CMAKE_CXX_STANDARD 17)

IF (APPLE)
    set(FLAGS "-Wall -undefined dynamic_lookup")
ENDIF ()

set(CMAKE_CXX_FLAGS "${FLAGS}")

include_directories(/third_party/**.c)
include_directories(/third_party/**.h)
add_compile_definitions(DART_SHARED_LIB=1)

set(SOURCE_FILES src/dart.cpp
        src/dart.h
        third_party/sqlite-amalgamation-3290000/sqlite3.c
        third_party/sqlite-amalgamation-3290000/sqlite3.h)

add_library(sqlite SHARED ${SOURCE_FILES})

IF (WIN32)
    include_directories("C:\\tools\\Dart-64\\dart-sdk\\include")
    target_link_libraries(sqlite "C:\\tools\\Dart-32\\dart-sdk\\bin\\dart.lib")
    set_target_properties(sqlite
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "..\\..\\lib\\src")
ELSEIF (APPLE)
    include_directories(/usr/local/opt/dart/libexec/include)
    set_target_properties(sqlite
            PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "../../lib/src/")
ELSEIF (CMAKE_SIZEOF_VOID_P MATCHES "8")
    include_directories(/usr/lib/dart/include)
    set_target_properties(sqlite
            PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "../../lib/src/")
ENDIF ()


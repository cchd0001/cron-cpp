cmake_minimum_required(VERSION 3.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/auto_bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/auto_lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/auto_lib)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/boost.cmake ) 
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/unittest++.cmake ) 

# compile options
if(WIN32)
    add_definitions(
        /Od
        /Mtd
        /Wall
    )
else()
    add_definitions( 
        -g
        -std=c++11
        -Wall
    )
endif()

project(Cron)
set(CRON_INCS  cron.h
    cron_err.h
    cron_parser.h
    cron_spec.h
    StringFunc.h
    cron_boost.h
    cron_const.h
)
set(CRON_SRCS  cron.cpp
    cron_err.cpp
    cron_parser.cpp
    cron_spec.cpp
    cron_const.cpp
)

add_library(CppCron STATIC ${CRON_SRCS} ${CRON_INCS})

add_subdirectory(test)
add_subdirectory(example)

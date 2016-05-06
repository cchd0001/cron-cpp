cmake_minimum_required(VERSION 3.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/auto_bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/auto_lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/auto_lib)

link_directories(
      ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)
# compile options
add_definitions(
    -g
    -std=c++11
    -Wall
)

project(Cron)

#add_subdirectory(example)

set(CRON_INCS  cron.h
    cron_err.h
    cron_parser.h
    cron_spec.h
    StringFunc.h
    cron_boost.h
)

set(CRON_SRCS  cron.cpp
    cron_err.cpp
    cron_parser.cpp
    cron_spec.cpp
)
add_library(CppCron STATIC ${CRON_SRCS} ${CRON_INCS})

#target_link_libraries(CppCron
#boost_system
#)

############################################################
#
# Test below

if(WIN32)

link_directories( 
E:/UnitTest++/unittest-cpp/builds/Debug
E:/boost.1.60/boost_1_60_0/bin.v2/libs/system/build/msvc-14.0/debug/link-static/threading-multi
E:/boost.1.60/boost_1_60_0/bin.v2/libs/date_time/build/msvc-14.0/debug/link-static/threading-multi
${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} 
E:/boost.1.60/boost_1_60_0/bin.v2/libs/regex/build/msvc-14.0/debug/link-static/threading-multi
)
include_directories(
E:/UnitTest++/unittest-cpp
E:/boost.1.60/boost_1_60_0
)

else()

link_directories( /usr/local/lib64 
                  /usr/local/lib
                  /usr/lib64
                  /usr/lib
                  ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
endif()

add_executable(Test test.cpp
    StringFunc_test.cpp
    cron_parser_test.cpp
    cron_spec_test.cpp
    cron_test.cpp
)

if(WIN32)
target_link_libraries(Test
	libboost_system-vc140-mt-gd-1_60.lib
	libboost_date_time-vc140-mt-gd-1_60.lib
	UnitTest++
	CppCron
)
else()
target_link_libraries(Test
    boost_system
    boost_date_time
    UnitTest++
    CppCron
    pthread
)
endif()

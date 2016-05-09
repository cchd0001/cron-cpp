
add_executable(Example helloworld.cpp)

if(WIN32)

    link_directories( 
        E:/boost.1.60/boost_1_60_0/bin.v2/libs/system/build/msvc-14.0/debug/link-static/threading-multi
        E:/boost.1.60/boost_1_60_0/bin.v2/libs/date_time/build/msvc-14.0/debug/link-static/threading-multi
        ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} 
        E:/boost.1.60/boost_1_60_0/bin.v2/libs/regex/build/msvc-14.0/debug/link-static/threading-multi
        )
    include_directories(
        E:/boost.1.60/boost_1_60_0
        ${CMAKE_SOURCE_DIR}
        )

    target_link_libraries(Example
        libboost_system-vc140-mt-gd-1_60.lib
        libboost_date_time-vc140-mt-gd-1_60.lib
        CppCron
        )
else()

    link_directories( /usr/local/lib64 
        /usr/local/lib
        /usr/lib64
        /usr/lib
        ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} 
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )

    include_directories(
        ${CMAKE_SOURCE_DIR}
        )
    
    target_link_libraries(Example
        boost_system
        boost_date_time
        CppCron
        pthread
        )
endif()

find_package(Boost REQUIRED ) #COMPONENTS system date_time)

if(Boost_FOUND)
    message(STATUS "Boost is ${BOOST_ROOT}")
    message(STATUS "Boost directories found at ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost libraries found at ${Boost_LIBRARY_DIRS}")
    message(STATUS "Boost component libraries to be linked are ${Boost_LIBRARIES}")
    message(STATUS "Boost version found is ${Boost_VERSION}")
    include_directories("${Boost_INCLUDE_DIRS}")
    link_directories("${Boost_LIBRARY_DIRS}")
elseif()
    message("Boost NOT Found!")
endif()


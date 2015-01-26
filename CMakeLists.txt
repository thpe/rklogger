cmake_minimum_required(VERSION 2.6)
#
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to Debug prior to calling PROJECT()
#
IF(DEFINED CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ELSE()
    SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ENDIF()
#set(CMAKE_INSTALL_PREFIX=/usr/local)
#INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/rklogger DESTINATION bin)
INSTALL(PROGRAMS src/bin/rklogger DESTINATION bin)
#SET(Boost_DEBUG 1)
#SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "/usr/include/boost")
#SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "/usr/include/boost/lib")

project(rklogger CXX)
add_definitions(-Weffc++ -Wextra -Wall -pedantic)
FIND_PACKAGE(Boost 1.42 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
include_directories(lib)
link_directories(${Boost_LIBRARY_DIR})
add_subdirectory(lib)
add_subdirectory(src)

set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
set(CPACK_SOURCE_IGNORE_FILES "/bin/;/.svn/;/.git/;~$;/CMakeCache.txt;/CMakeFiles/;/CPackSourceConfig.cmake;/CPackConfig.cmake;/cmake_install.cmake;/Makefile;${CPACK_SOURCE_IGNORE_FILES}")
include(CPack)


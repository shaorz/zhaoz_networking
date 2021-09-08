# CMakeList.txt : CMake project for tcp_server, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("tcp_server")

ADD_DEFINITIONS("-DBOOST_ALL_NO_LIB")
set(CMAKE_CXX_STANDARD 17)
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost 1.76.0)
if(BOOST_FOUND)
    # Add source to this project's executable.
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable (tcp_server "tcp_server.cpp" "header/tcp_server.h")
endif()
# TODO: Add tests and install targets if needed.

# CMakeList.txt : CMake project for udp_server, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("udp_server")

# Add source to this project's executable.
add_executable (udp_server "udp_server.cpp" "udp_server.h")
if(WIN32)
  target_link_libraries(udp_server wsock32 ws2_32)
endif()
# TODO: Add tests and install targets if needed.

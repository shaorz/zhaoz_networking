# CMakeList.txt : CMake project for udp_client, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("udp_client")

# Add source to this project's executable.
add_executable (udp_client "udp_client.cpp" "udp_client.h")
if(WIN32)
  target_link_libraries(udp_client wsock32 ws2_32)
endif()
# TODO: Add tests and install targets if needed.

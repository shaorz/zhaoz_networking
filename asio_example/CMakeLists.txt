# CMakeList.txt : CMake project for asio_example, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("asio_example")

# Add source to this project's executable.
ADD_DEFINITIONS("-DBOOST_ALL_NO_LIB")

# add_library( mysharedlib SHARED "headers/asio_example.h" "headers/net_common.h" "headers/net_message.h" "headers/concurrent_queue.h" "headers/net_connection.h" "headers/net_client.h" "headers/net_server.h" "headers/message_type.h" )

set(CMAKE_CXX_STANDARD 17)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.36.0)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	message("BOOST LIBRARIES LOCATION: " ${Boost_LIBRARIES})
	add_executable ( asio_example "asio_example.cpp" "headers/asio_example.h" "headers/net_common.h" "headers/net_message.h" "headers/concurrent_queue.h" "headers/net_connection.h" "headers/net_client.h" "headers/net_server.h" "headers/message_type.h")
	# target_link_libraries( asio_example mysharedlib )


	add_executable ( simple_server "simple_server.cpp" "headers/asio_example.h" "headers/net_common.h" "headers/net_message.h" "headers/concurrent_queue.h" "headers/net_connection.h" "headers/net_client.h" "headers/net_server.h" "headers/message_type.h")
	# target_link_libraries( simple_server mysharedlib )
	
	add_executable ( simple_client "simple_client.cpp" "headers/asio_example.h" "headers/net_common.h" "headers/net_message.h" "headers/concurrent_queue.h" "headers/net_connection.h" "headers/net_client.h" "headers/net_server.h" "headers/message_type.h")
	# target_link_libraries( simple_client mysharedlib )
	# set_target_properties( asio_example simple_client simple_server PROPERTIES LINKER_LANGUAGE CXX )
endif()
# TODO: Add tests and install targets if needed.

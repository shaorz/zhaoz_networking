#pragma once

#include <iostream>
#include <memory> // shared_ptr unique_ptr
//#include <thread>  // 
//#include <mutex>
#include <deque>
//#include <optional>
//#include <vector>
//#include <algorithm>
//#include <chrono>
//#include <cstdint>

#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

#include "concurrent_queue.h"
#include "C17_thread_pool.h"


namespace zhaoz {
	namespace net {
		namespace tcp {
			
			template<typename MessageType> class IServer {
			private:
			protected:
				bool is_running;
				int port;

				moodycamel::ConcurrentQueue<MessageType> incomingMessageQueue;
				moodycamel::ConcurrentQueue<MessageType> outgoingMessageQueue;

				std::deque<std::shared_ptr<>> clients;

				boost::asio::io_context io_context;
				boost::asio::ip::tcp::socket


			public:
				IServer(int);

				virtual IServer() {
					disconnect();
				}

				void start();

				void restart();

				void disconnect();
			};

		}
	}
}
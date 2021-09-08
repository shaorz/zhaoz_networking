#pragma once

#include "net_connection.h"

namespace olc {
	namespace net {
		
		template<typename T>
		class IClient {
		private:
			// thread safe messsage queue caching incoming message from server
			concurrent_queue<owned_message<T>> m_msg_queue_in;
		
		protected:
			// asio context handles the data transfer
			boost::asio::io_context m_context;
			
			// .. but it requires a thread on its own to execute work commands
			std::thread thread_context;
			
			//client owns a single connection instance, which handles data transfer
			std::unique_ptr<connection<T>> m_connection;
		
		public:
			IClient() {}

			virtual ~IClient() {
				disconnect();
			}

			bool connect(const std::string& host, const uint16_t port) {
				try {
					boost::asio::ip::tcp::resolver resolver(m_context);
					boost::asio::ip::tcp::resolver::results_type m_endpoint = resolver.resolve(host, std::to_string(port));

					m_connection = std::make_unique<connection<T>>(owner::client, m_context, boost::asio::ip::tcp::socket(m_context), m_msg_queue_in);
					m_connection -> connect_to_server(m_endpoint);
					thread_context = std::thread(
						[this]() {m_context.run();  }
					);

				}
				catch (std::exception& e) {
					std::cerr << "client connection exception: " << e.what() << std::endl;
					return false;
				}

				return true;
			}

			void disconnect() {
				if (is_connected())
					m_connection -> disconnect();

				// stop the context from running
				m_context.stop();
				// stop the thread from running
				if (thread_context.joinable())
					thread_context.join();
				// destroy the connection obj
				m_connection.release();
			}

			bool is_connected() {
				if (m_connection)
					return m_connection -> is_connected();
				return false;
			}

			concurrent_queue<owned_message<T>>& incoming_msg_queue() {
				return m_msg_queue_in;
			}

			void send_msg(const message<T>& msg) {
				if (is_connected())
					m_connection->send_msg(msg);
			}
		};	
	}
}
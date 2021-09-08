#pragma once

#include "net_common.h"
#include "net_message.h"
#include "concurrent_queue.h"

namespace olc {
	namespace net {

		enum class owner {
			server,
			client
		};

		// CLASS TEMPLATE enable_shared_from_this
		template<typename T>
		class connection : public std::enable_shared_from_this<connection<T>> { // provide member functions that create shared_ptr to 'this'
		private:
			// ASYNC - Prime context to write a message header
			void write_header() {
				// If this function is called, we know the outgoing message queue must have 
				// at least one message to send. So allocate a transmission buffer to hold
				// the message, and issue the work - asio, send these bytes
				boost::asio::async_write(m_socket, boost::asio::buffer(&m_msg_queue_out.front().header, sizeof(message_header<T>)), 
				[this](boost::system::error_code ec, std::size_t length) {
					// asio has now sent the bytes
					if (!ec) {
						if (m_msg_queue_out.front().header.size > 0) {
							write_body();
						}
						else {
							// done with the current msg
							m_msg_queue_out.pop_front();
							if (!m_msg_queue_out.empty()) {
								write_header();
							}
						}
					}
					else {
						std::cout << "[" << id << "] write header failed " << ec << std::endl;
						m_socket.close();
					}
				});
			}

			void write_body() {
				boost::asio::async_write(m_socket, boost::asio::buffer(m_msg_queue_out.front().body.data(), m_msg_queue_out.front().body.size()), 
				[this](boost::system::error_code ec, std::size_t length) {
					// asio has completed writing the body
					if (!ec) {
						// pop out the message queue
						m_msg_queue_out.pop_front();

						if (!m_msg_queue_out.empty())
							read_header();

					}
					else {
						std::cout << "[" << id << "] write body failed " << ec << std::endl;
						m_socket.close();
					}
				});
			}

			// ASYNC - Prime context to read a message header
			void read_header() {
				 // if this function is called, we are expecting asio to wait until it receives enough bytes to form
				// a message header, with a fixed size. in fact, we will construct the msg in a temporary msg object
				// as it's convenient to work with
				boost::asio::async_read(m_socket, boost::asio::buffer(&m_msg_in_cache, sizeof(message_header<T>)),
					[this](boost::system::error_code ec, std::size_t length) {
						if (!ec) {
							if (m_msg_in_cache.header.size > 0) {
								m_msg_in_cache.body.resize(m_msg_in_cache.header.size);
								read_body();
							}
							else {
								add_to_incoming_msg_queue();
							}
						}
						else {
							std::cout << "[" << id << "] read header failed " << ec << std::endl;
							m_socket.close();
						}
					});
			}

			// ASYNC - Prime asio context to read message body
			void read_body() {
				boost::asio::async_read(m_socket, boost::asio::buffer(m_msg_in_cache.body.data(), m_msg_in_cache.body.size()),
					[this](boost::system::error_code ec, std::size_t length) {
						if (!ec) {
							add_to_incoming_msg_queue();
						}
						else {
							std::cout << "[" << id << "] read body failed " << ec << std::endl;
							m_socket.close();
						}
					});
			}

			// once a full message is received, add it to the queue
			void add_to_incoming_msg_queue() {
				if (m_nOwnerType == owner::server)
					m_msg_queue_in.push_back({ this->shared_from_this(), m_msg_in_cache });
				else
					m_msg_queue_in.push_back({ nullptr, m_msg_in_cache });
				// prime the header for the next message
				read_header();
			}

		protected:
			// Each connection has a unique socket to a remote 
			boost::asio::ip::tcp::socket m_socket;

			// This context is shared with the whole asio instance
			boost::asio::io_context& m_context;

			// This queue holds all messages to be sent to the remote side
			// of this connection
			concurrent_queue<message<T>> m_msg_queue_out;

			// This references the incoming queue of the parent object
			concurrent_queue<owned_message<T>>& m_msg_queue_in;

			// mutable message buffer for sending out
			mutable message<T> m_msg_in_cache;

			owner m_nOwnerType = owner::server;

			uint32_t id = 0;

		public:

			// Constructor: Specify Owner, connect to context, transfer the socket
			//	Provide reference to incoming message queue
			connection(owner parent, boost::asio::io_context& asioContext, boost::asio::ip::tcp::socket socket, concurrent_queue<owned_message<T>>& qIn)
				: m_context(asioContext), m_socket(std::move(socket)), m_msg_queue_in(qIn), m_nOwnerType(parent) {
			
			}

			virtual ~connection() { }

			uint32_t get_id() const {
				return id;
			}

			void connect_to_client(uint32_t uid = 0) {
				if (m_nOwnerType == owner::server) {
					if (m_socket.is_open()) {
						id = uid;
						read_header();
					}
				}
			}

			void connect_to_server(boost::asio::ip::tcp::resolver::results_type& endpoint) {
				if (m_nOwnerType == owner::client) {
					boost::asio::async_connect(m_socket, endpoint, 
						[this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint endpoint){
							if (!ec)
								read_header();
						});
				}
			}

			void disconnect() {
				if (is_connected())
					// injecting a new task into the asio context
					boost::asio::post(m_context, 
					[this]() {
						m_socket.close();
					});
			}

			bool is_connected() const {
				return m_socket.is_open();
			}

			// prime the connection to wait for incoming messages
			void start_listening() {}

			// ASYNC - send a message, 1 - 1
			void send_msg(const message<T>& msg) {
				boost::asio::post(m_context, 
				[this, msg]() {
						// If the queue has a message in it, then we must 
						// assume that it is in the process of asynchronously being written.
						// Either way add the message to the queue to be output. If no messages
						// were available to be written, then start the process of writing the
						// message at the front of the queue.
						bool bWritingMessage = !m_msg_queue_out.empty();
						m_msg_queue_out.push_back(msg);
						// we will only write new messages when the queue was empty, before we added a msg
						if (!bWritingMessage)
						{
							write_header();
						}
				});
			}
		};
	}
}
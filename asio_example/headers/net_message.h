#pragma once

#include "net_common.h"

namespace olc {
	namespace net {
		
		template<typename _enum_class_T>
		struct message_header {
			_enum_class_T id {};
			size_t size = 0;
		};

		// Message Body contains a header and a std::vector, containing raw bytes
		// of infomation. This way the message can be variable length, but the size
		// in the header must be updated.
		template<typename _enum_class_T>
		struct message {
			message_header<_enum_class_T> header {};
			std::vector<uint8_t> body;

			/*uint32_t size() const {
				return sizeof(header) + body.size();
			}*/
			// returns size of entire message packet in bytes
			size_t size() const {
				return body.size();
			}

			friend std::ostream& operator << (std::ostream& ct, const message<_enum_class_T>& data) {
				ct << "MessageID: " << static_cast<int>(data.header.id) << ", Size: " << data.header.size << std::endl;
				return ct;
			}

			template<typename datatype>
			friend message<_enum_class_T>& operator << (message<_enum_class_T>& in_msg,  const datatype& data) {
				// static_assert(std::is_standard_layout<datatype>::value, );
				static_assert(std::is_standard_layout_v<datatype>, "input data is too complex to be pushed");

				size_t size_prev = in_msg.body.size();
				in_msg.body.resize(size_prev + sizeof(datatype));
				
				std::memcpy(in_msg.body.data() + size_prev, &data, sizeof(datatype));
				
				// sync message header size
				in_msg.header.size = in_msg.size();
				// std::cout << "new size after adding " << data << " is " << in_msg.size() << std::endl;
				return in_msg;
			}

			template<typename datatype>
			friend message<_enum_class_T>& operator >> (message<_enum_class_T>& out_msg, datatype& data) {
				static_assert(std::is_standard_layout_v<datatype>, "output data type is too complex to be pushed");

				size_t size_new = out_msg.body.size() - sizeof(datatype);
				std::memcpy(&data, out_msg.body.data() + size_new, sizeof(datatype));
				out_msg.body.resize(size_new);

				// sync up header size property
				out_msg.header.size = out_msg.size();
				// std::cout << "new size after removing " << data << " is " << size_new << std::endl;
				return out_msg;
			}
		};

		// An "owned" message is identical to a regular message, but it is associated with
		// a connection. On a server, the owner would be the client that sent the message, 
		// on a client the owner would be the server.
		template<typename _enum_class_T>
		class connection;

		template<typename _enum_class_T>
		struct owned_message {
			std::shared_ptr<connection<_enum_class_T>> remote = nullptr;
			message<_enum_class_T> msg;

			/*friend std::ostream& operator << (ostream& os, const owned_message& msg) {
				os << "owned message: " << msg.msg;
				return os;
			}*/
		};
	}
}

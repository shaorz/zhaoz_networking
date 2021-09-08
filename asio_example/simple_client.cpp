#include "headers/asio_example.h"
#include <iostream>

class CustomClient : public olc::net::IClient<MessageType> {
private:
protected:
public:
	void PingServer()
	{
		olc::net::message<MessageType> msg;
		msg.header.id = MessageType::ServerPing;

		// Caution with this...
		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

		msg << timeNow;
		send_msg(msg);
	}

	void MessageAll()
	{
		olc::net::message<MessageType> msg;
		msg.header.id = MessageType::MessageAll;
		send_msg(msg);
	}
};

int main() {

	CustomClient client;
	client.connect("127.0.0.1", 52111);

	bool key[3] = { false, false, false };
	bool old_key[3] = { false, false, false };

	bool running = true;
	while (running) {
		if (GetForegroundWindow() == GetConsoleWindow()) {
			key[0] == GetAsyncKeyState('1') & 0x8000;
			key[1] == GetAsyncKeyState('2') & 0x8000;
			key[2] == GetAsyncKeyState('3') & 0x8000;
		}
	
		if (key[0] && !old_key[0])
			client.PingServer();
		if (key[1] && !old_key[1])
			client.MessageAll();
		if (key[2] && !old_key[2])
			running = false;

		for (int i = 0; i < 3; ++i)
			old_key[i] = key[i];

		if (client.is_connected()) {
			if (!client.incoming_msg_queue().empty()) {
				auto msg = client.incoming_msg_queue().pop_front().msg;

				switch (msg.header.id)
				{

				case MessageType::ServerAccept: {
					std::cout << "Server accepted the connection" << std::endl;
					break;
				}
				case MessageType::ServerPing: {
					std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
					std::chrono::system_clock::time_point then;
					msg >> then;
					std::cout << "Ping: " << std::chrono::duration<double>(timeNow - then).count() << std::endl;
					break; 
				}
				case MessageType::ServerMessage: {
					uint32_t client_id;
					msg >> client_id;
					std::cout << "Hello from [" << client_id << "]\n";

					break; 
				}
				default:
					break;
				}
			}
		}
		else {
			std::cout << "Server goes down" << std::endl;
			running = false;
		}

	}


	return 0;
}
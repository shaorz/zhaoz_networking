#include "headers/asio_example.h"

class CustomServer : public olc::net::server_interface<MessageType> {
private:
protected:
	// Called when a client connects, you can veto the connection by returning false
	virtual bool OnClientConnect(std::shared_ptr<olc::net::connection<MessageType>> client) override
	{
		olc::net::message<MessageType> msg;
		msg.header.id = MessageType::ServerAccept;
		client->send_msg(msg);
		return true;
	}

	// Called when a client appears to have disconnected
	virtual void OnClientDisconnect(std::shared_ptr<olc::net::connection<MessageType>> client)
	{
		std::cout << "Removing client [" << client->get_id() << "]" << std::endl;
	}

	// Called when a message arrives
	virtual void OnMessage(std::shared_ptr<olc::net::connection<MessageType>> client, olc::net::message<MessageType>& msg)
	{
		switch (msg.header.id)
		{
		case MessageType::ServerPing:
		{
			std::cout << "[" << client->get_id() << "]: Server Ping\n";
			client->send_msg(msg);
		}
		break;
		case MessageType::MessageAll:
		{
			std::cout << "[" << client->get_id() << "]: Message All" << std::endl;
			olc::net::message<MessageType> msg;
			msg.header.id = MessageType::ServerMessage;
			msg << client->get_id();
			MessageAllClients(msg, client);
		
		}
		break;
		default:
			break;
		}
	}

public:
	CustomServer(uint16_t nPort) : olc::net::server_interface<MessageType>(nPort) {}
};

int main() {

	CustomServer server(52111);
	server.Start();
	bool running = true;
	while (running) {
		server.Update(-1, true);
	
	}
	return 0;
}

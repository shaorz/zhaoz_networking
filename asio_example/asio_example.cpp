// asio_example.cpp : Defines the entry point for the application.
//

#include "headers/asio_example.h"


using namespace std;
using namespace boost::asio;
using namespace chrono_literals;

enum class custom_msg_type : uint16_t {
	FIRE_BULLET,
	PLAYER_MOVE
};

struct _for_test {
	int x, y, z;
	friend std::ostream& operator << (std::ostream& ct, _for_test& data) {
		ct << "_for_test {x: " << data.x << ", y: " << data.y << ", z: " << data.z << "}" << endl;
		return ct;
	}
};

void fetch_data(vector<char>& r_buffer, ip::tcp::socket &skt) { // , boost::system::error_code &ec
	skt.async_read_some(buffer(r_buffer.data(), r_buffer.size()), 
		[&] (error_code ec, size_t length) {
			if (!ec) {
				cout << "\n\nReading: " << length << "bytes\n\n";
				for (int i = 0; i < length; ++i) {
					cout << r_buffer[i];
				}
				fetch_data(r_buffer, skt);
			}
		});
}

void test_http_get();
void test_custom_message();

int main()
{
	test_custom_message();

	return 0;
}

void test_custom_message() {
	olc::net::message<custom_msg_type> custom_msg;
	custom_msg.header.id = custom_msg_type::FIRE_BULLET;
	custom_msg.header.size = 0;
	
	char* str1 = "Hello World";
	char* str2 = "\n\nI'm Roger";
	int idata = 11;
	float fdata = 520.99f;
	double ddata = 520.99999999;
	_for_test sdata[10];

	custom_msg << idata << fdata << ddata << sdata; // << str1;
	cout << custom_msg << endl;
	idata = 0;
	fdata = 0;
	ddata = 0;

	custom_msg >> sdata >> ddata >> fdata >> idata; // 
}



void test_http_get() {
	vector<char> read_buffer(256);
	boost::system::error_code ec;

	// io context is required for asio to work, if asio doesnt' have any work to do, it will exit immediately
	io_context context;

	// create some fake work for the context to run || asio to do || such that asio won't exit immediately
	io_context::work idle_work(context);

	// thread the async read 
	thread read_thread = thread(
		[&]() {
			context.run();
		}
	);

	// short response:
	ip::tcp::endpoint endpoint(ip::make_address("93.184.216.34", ec), 80);
	// large response:
	ip::tcp::endpoint long_endpoint(ip::make_address("51.38.81.49", ec), 80);
	// ip::tcp::endpoint local_endpoint(ip::make_address("127.0.0.1", ec), 8080);

	ip::tcp::socket socket(context);
	socket.connect(long_endpoint, ec);

	if (!ec) {
		cout << "connected to example endpoint: " << long_endpoint << endl;
	}
	else {
		cout << "failed to connect to address: " << ec.message() << endl;
	}

	if (socket.is_open()) {

		fetch_data(read_buffer, socket);

		string sRequest = "GET /index.html HTTP/1.1\r\n"
			"Host: example.com\r\n"
			"Connection: close\r\n\r\n";
		socket.write_some(buffer(sRequest.data(), sRequest.size()), ec);

		// this_thread::sleep_for(200ms); 
		//socket.wait(socket.wait_read);
		//size_t byte = socket.available(); // unsigned long long 
		//cout << "bytes available: " << byte << endl;

		//if (byte > 0) {
		//	vector<char> read_buffer(byte);
		//	socket.read_some(buffer(read_buffer.data(), read_buffer.size()), ec);
		//	
		//	for (auto c : read_buffer) {
		//		cout << c;
		//	}
		//}
		// 
		// some other stuff for application to do before it exist
		this_thread::sleep_for(20000ms);

		context.stop();
		if (read_thread.joinable())
			read_thread.join();

	}
}
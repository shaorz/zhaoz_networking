// udp_server.cpp : Defines the entry point for the application.
//

#include "udp_server.h"

using namespace std;

const float32 ACCELERATION = 0.01f;
const float32 TURN_SPEED = 0.01f;
const float32 MAX_SPEED = 1.0f;

int main()
{
	WORD winsocket_version = 0x202;
	WSADATA winsocket_data;
	if (WSAStartup(winsocket_version, &winsocket_data)) {
		printf("WSAStartup failed: %d", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	int address_family = AF_INET;
	int type = SOCK_DGRAM;
	int protocol = IPPROTO_UDP;
	SOCKET sock = socket(address_family, type, protocol);
	if ( sock == INVALID_SOCKET) {
		printf("socket constructor failed: %d", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	SOCKADDR_IN local_address;
	local_address.sin_family = address_family;
	local_address.sin_port = htons(PORT);
	local_address.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock, (SOCKADDR*) &local_address, sizeof(local_address)) == SOCKET_ERROR) {
		printf("socket binding failed: %d", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	char buffer[SOCKET_BUFFER_SIZE];
	float32 player_x = 0;
	float32 player_y = 0;
	float32 player_facing = 0;
	float32 player_speed = 0;

	bool is_running = 1;

	while (is_running) {
		int flag = 0;
		SOCKADDR_IN from;
		int from_size = sizeof(from);
		int bytes_received = recvfrom(sock,		// input socket
			buffer,													// output buffer
			SOCKET_BUFFER_SIZE,						// input length of buffer
			flag,														// input flags
			(SOCKADDR*)&from,							// output client IP
			&from_size);										// input+output client IP length

		// printf("received datagram:%d... \n", bytes_received);

		if (bytes_received == SOCKET_ERROR) {
			printf("socket data receiving failed: %d", WSAGetLastError());
			break;
		}

		// processing package
		char client_input = buffer[0];

		printf("%d.%d.%d.%d:%d -- %c\n",
			from.sin_addr.s_net, from.sin_addr.s_host, from.sin_addr.s_lh, from.sin_addr.s_impno,
			from.sin_port, client_input);
		// wasd movement
		/*switch (client_input) {
		case 'w':
			++player_y;
			break;
		case 's':
			--player_y;
			break;
		case 'a':
			--player_x;
			break;
		case 'd':
			++player_x;
			break;
		case 'q':
			is_running = 0;
		default:
			printf("undefined input %c \n", client_input);
			break;
		}*/

		if (client_input & 0x1) { // forward
			player_speed += ACCELERATION;
			if (player_speed > MAX_SPEED)
				player_speed = MAX_SPEED;
		}
		if (client_input & 0x2) { // backward
			player_speed -= ACCELERATION;
			if (player_speed < -MAX_SPEED)
				player_speed = -MAX_SPEED;
		}
		if (client_input & 0x4) { // left
			player_facing -= TURN_SPEED;
		}
		if (client_input & 0x8) { // left
			player_facing += TURN_SPEED;
		}

		player_x += player_speed * sinf(player_facing);
		player_y += player_speed * cosf(player_facing);

		//create state packet
		int32 write_index = 0;
		memcpy(&buffer[write_index], &player_x, sizeof(player_x));
		write_index += sizeof(player_x);

		memcpy(&buffer[write_index], &player_y, sizeof(player_y));
		write_index += sizeof(player_y);

		memcpy(&buffer[write_index], &player_facing, sizeof(player_facing));
		write_index += sizeof(player_facing);
		// memcpy(&buffer[write_index], &is_running, sizeof(is_running));

		// send datagram back to client
		flag = 0;
		SOCKADDR* to = (SOCKADDR*)&from;
		int to_length = sizeof(from);
		if (sendto(sock, buffer, write_index, flag, to, to_length) == SOCKET_ERROR ) {
			printf("sendto failed: %d", WSAGetLastError());
			WSACleanup();
			return 0;
		}
	}
	WSACleanup();
	return 0;
}

// udp_client.cpp : Defines the entry point for the application.
//

#include "udp_client.h"
#include "../udp_server/udp_server.h"

using namespace std;

int main()
{
	WORD winsocket_verison = 0x202;
	WSADATA winsocket_data;
	if (WSAStartup(winsocket_verison, &winsocket_data)) {
		printf("WSAStartup failed: %d", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	int address_family = AF_INET;
	int type = SOCK_DGRAM;
	int protocol = IPPROTO_UDP;
	SOCKET sock = socket(address_family, type, protocol);

	if (sock == INVALID_SOCKET) {
		printf("socket constructor failed: %d", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	SOCKADDR_IN server_address;
	server_address.sin_family = address_family;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	char buffer[SOCKET_BUFFER_SIZE];
	int player_x, player_y;
	printf("type w a s d to move and q to quit the game\n");
	bool is_running = 1;
	while (is_running) {
		// get input
		scanf_s("%c\n", &buffer[0], 1);

		// send to server
		int buffer_length = 1; 
		int flag = 0;
		if (sendto(sock, buffer, buffer_length, flag, (SOCKADDR*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
			printf("send to server failed: %d", WSAGetLastError());
			WSACleanup();
			return 0;
		}

		// wait for reply
		flag = 0;
		SOCKADDR_IN from;
		int from_size = sizeof(from);
		int byte_received = recvfrom(sock, buffer, SOCKET_BUFFER_SIZE, flag, (SOCKADDR*) &from, &from_size);

		if (byte_received == SOCKET_ERROR) {
			printf("receive from server failed due to: %d", WSAGetLastError());
			WSACleanup();
			return 0;
		}

		int read_index = 0;
		memcpy(&player_x, &buffer[read_index], sizeof(player_x));
		read_index += sizeof(player_x);

		memcpy(&player_y, &buffer[read_index], sizeof(player_y));
		read_index += sizeof(player_y);

		memcpy(&is_running, &buffer[read_index], sizeof(is_running));

		printf("x: %d, y: %d, is running: %d\n", player_x, player_y, is_running);
	}	
	

	printf("quit games");
	WSACleanup();
	return 0;
}

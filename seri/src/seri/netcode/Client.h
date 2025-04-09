#pragma once

#include "seri/logging/Logger.h"

#include <cstring>
#include <iostream>
#include <winsock2.h>

namespace seri::netcode
{
	class Client
	{
	public:
		void Init()
		{
			WSADATA wsaData;
			SOCKET sockfd;
			sockaddr_in serverAddr{};
			char buffer[1024];

			if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			{
				LOGGER(error, "WSAStartup failed");
				return;
			}

			sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (sockfd == INVALID_SOCKET)
			{
				LOGGER(error, "Socket creation failed");
				WSACleanup();
				return;
			}

			serverAddr.sin_family = AF_INET;
			serverAddr.sin_port = htons(_port);
			serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

			std::string msg = "Ping!";
			sendto(sockfd, msg.c_str(), msg.length(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

			int serverLen = sizeof(serverAddr);
			int recvLen = recvfrom(sockfd, buffer, sizeof(buffer), 0, (sockaddr*)&serverAddr, &serverLen);

			if (recvLen != SOCKET_ERROR)
			{
				LOGGER(error, "Server replied: " << recvLen);
			}

			closesocket(sockfd);
			WSACleanup();
		}

	private:
		unsigned int _port{ 5200 };

	};
}

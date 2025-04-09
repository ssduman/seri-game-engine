#pragma once

#include "seri/logging/Logger.h"

#include <cstring>
#include <iostream>
#include <winsock2.h>

namespace seri::netcode
{
	class Server
	{
	public:
		void Init()
		{
			WSADATA wsaData;
			SOCKET sockfd;
			sockaddr_in serverAddr{}, clientAddr{};
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
			serverAddr.sin_addr.s_addr = INADDR_ANY;
			serverAddr.sin_port = htons(_port);

			if (bind(sockfd, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
			{
				LOGGER(error, "Bind failed");
				closesocket(sockfd);
				WSACleanup();
				return;
			}

			LOGGER(info, "Server is running on port: " << _port);

			while (true)
			{
				int clientLen = sizeof(clientAddr);
				int recvLen = recvfrom(sockfd, buffer, sizeof(buffer), 0, (sockaddr*)&clientAddr, &clientLen);
				if (recvLen == SOCKET_ERROR)
				{
					LOGGER(info, "recvfrom failed");
					break;
				}

				std::cout << "Received: " << recvLen << std::endl;

				std::string reply = "Pong!";
				sendto(sockfd, reply.c_str(), reply.length(), 0, (sockaddr*)&clientAddr, clientLen);
			}

			closesocket(sockfd);
			WSACleanup();
		}

	private:
		unsigned short _port{ 5200 };

	};
}

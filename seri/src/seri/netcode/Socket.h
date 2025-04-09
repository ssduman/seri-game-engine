#pragma once

#include "seri/logging/Logger.h"

#include <cstring>
#include <iostream>
#include <ws2tcpip.h>
#include <winsock2.h>

namespace seri::netcode
{
	enum SocketType
	{
		udp = IPPROTO_UDP,
		tcp = IPPROTO_TCP,
	};

	struct RemoteEndpoint
	{
		RemoteEndpoint()
		{
			FillDefaults();
		}

		const char* ip;
		unsigned int port;

		void FillDefaults()
		{
			ip = "localhost";
			port = 5200;
		}
	};

	class Socket
	{
	public:
		Socket(SocketType st)
		{
			char buffer[1024];
			if (WSAStartup(MAKEWORD(2, 2), &_wsaData) != 0)
			{
				_success = false;
				LOGGER(error, "[netcode] WSAStartup failed");
				return;
			}

			_sockfd = socket(AF_INET, SOCK_DGRAM, st);
			if (_sockfd == INVALID_SOCKET)
			{
				LOGGER(error, "[netcode] socket creation failed");
				WSACleanup();
				return;
			}

			_success = true;
		}

		~Socket() = default;

		bool Bind(RemoteEndpoint re)
		{
			if (!_success)
			{
				return;
			}

			sockaddr_in serverAddr{};

			serverAddr.sin_family = AF_INET;
			serverAddr.sin_port = htons(re.port);
			serverAddr.sin_addr.s_addr = INADDR_ANY; // inet_addr(re.ip);

			InetPton(AF_INET, (const WCHAR*)re.ip, &serverAddr.sin_addr.s_addr);

			if (bind(_sockfd, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
			{
				_success = false;
				LOGGER(error, "[netcode] bind failed");
				closesocket(_sockfd);
				WSACleanup();
				return;
			}

			LOGGER(info, "[netcode] server is running on port: " << re.port);
		}

		void Connect()
		{

		}

		bool Available()
		{
		}

		void Listen(int maxListener = 1)
		{
		}

	private:
		bool _success{ false };

		WSADATA _wsaData;
		SOCKET _sockfd{ 0 };

	};

}

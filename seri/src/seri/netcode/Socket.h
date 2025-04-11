#pragma once

#include "seri/logging/Logger.h"

#include <cstring>
#include <iostream>
#include <ws2tcpip.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

namespace seri::netcode
{
	enum SocketType
	{
		udp = IPPROTO_UDP,
		tcp = IPPROTO_TCP,
	};

	struct RemoteEndpoint
	{
		const char* ip;
		unsigned int port;
	};

	class Socket
	{
	public:
		Socket(SocketType st)
		{
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

			static u_long mode = 1; // 1 = non-blocking
			ioctlsocket(_sockfd, FIONBIO, &mode);

			_success = true;
		}

		~Socket()
		{
			if (_success)
			{
				closesocket(_sockfd);
				WSACleanup();
			}
		}

		bool Bind(RemoteEndpoint re)
		{
			if (!_success)
			{
				return false;
			}

			_serverAddr.sin_family = AF_INET;
			_serverAddr.sin_port = htons(re.port);
			_serverAddr.sin_addr.s_addr = INADDR_ANY; // inet_addr(re.ip);

			InetPton(AF_INET, (const WCHAR*)re.ip, &_serverAddr.sin_addr.s_addr);

			if (bind(_sockfd, (sockaddr*)&_serverAddr, sizeof(_serverAddr)) == SOCKET_ERROR)
			{
				_success = false;
				LOGGER(error, "[netcode] bind failed");
				closesocket(_sockfd);
				WSACleanup();
				return false;
			}

			LOGGER(info, "[netcode] server is running on port: " << re.port);

			return true;
		}

		bool Connect(RemoteEndpoint re)
		{
			if (!_success)
			{
				return false;
			}

			_serverAddr.sin_family = AF_INET;
			_serverAddr.sin_port = htons(re.port);
			_serverAddr.sin_addr.s_addr = inet_addr(re.ip);

			InetPton(AF_INET, (const WCHAR*)re.ip, &_serverAddr.sin_addr.s_addr);

			LOGGER(info, "[netcode] client connected to port: " << re.port);

			return true;
		}

		bool Available()
		{
			sockaddr_in clientAddr{};

			char temp[1];
			int clientLen = sizeof(clientAddr);
			int res = recvfrom(_sockfd, temp, 1, MSG_PEEK, (sockaddr*)&clientAddr, &clientLen);
			if (res == SOCKET_ERROR)
			{
				int err = WSAGetLastError();
				return err == WSAEWOULDBLOCK ? false : true;
			}
			return true;
		}

		void Listen(int maxListener = 1)
		{
			sockaddr_in clientAddr{};

			if (Available())
			{
				int clientLen = sizeof(clientAddr);
				int recvLen = recvfrom(_sockfd, _buffer, sizeof(_buffer), 0, (sockaddr*)&clientAddr, &clientLen);
				if (recvLen == SOCKET_ERROR)
				{
					LOGGER(info, "[netcode] recvfrom failed");
					return;
				}

				char ipStr[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, &(clientAddr.sin_addr), ipStr, INET_ADDRSTRLEN);
				int port = ntohs(clientAddr.sin_port);

				LOGGER(info, "[netcode] message from: " << ipStr << ":" << port << ", received: " << recvLen);

				Send(clientAddr);
			}
		}

		void Send(const sockaddr_in& clientAddr)
		{
			std::string reply = "pong!";
			int clientLen = sizeof(clientAddr);
			sendto(_sockfd, reply.c_str(), reply.length(), 0, (sockaddr*)&clientAddr, clientLen);
		}

		void SendToServer()
		{
			std::string reply = "ping!";
			int clientLen = sizeof(_serverAddr);
			sendto(_sockfd, reply.c_str(), reply.length(), 0, (sockaddr*)&_serverAddr, clientLen);
		}

	private:
		bool _success{ false };

		WSADATA _wsaData;
		SOCKET _sockfd{ 0 };

		sockaddr_in _serverAddr{};

		char _buffer[65536]{};

	};

}

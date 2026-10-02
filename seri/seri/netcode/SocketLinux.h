#pragma once

#include "seri/netcode/Socket.h"
#include "seri/logging/Logger.h"

#include <string>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace seri::netcode
{
	class SocketLinux : public Socket
	{
	public:
		SocketLinux(SocketType st)
		{
			_sockfd = socket(AF_INET, SOCK_DGRAM, st == SocketType::udp ? IPPROTO_UDP : IPPROTO_TCP);
			if (_sockfd < 0)
			{
				LIB_LOGGER(error, netcode) << "socket creation failed";
				return;
			}

			fcntl(_sockfd, F_SETFL, fcntl(_sockfd, F_GETFL, 0) | O_NONBLOCK);

			_success = true;
		}

		~SocketLinux() override
		{
			if (_success)
			{
				close(_sockfd);
			}
		}

		bool Bind(RemoteEndpoint re) override
		{
			if (!_success)
			{
				return false;
			}

			_serverAddr.sin_family = AF_INET;
			_serverAddr.sin_port = htons(re.port);
			_serverAddr.sin_addr.s_addr = INADDR_ANY;

			inet_pton(AF_INET, re.ip, &_serverAddr.sin_addr.s_addr);

			if (bind(_sockfd, (sockaddr*)&_serverAddr, sizeof(_serverAddr)) < 0)
			{
				_success = false;
				LIB_LOGGER(error, netcode) << "bind failed";
				close(_sockfd);
				return false;
			}

			LIB_LOGGER(info, netcode) << "server is running on port: " << re.port;

			return true;
		}

		bool Connect(RemoteEndpoint re) override
		{
			if (!_success)
			{
				return false;
			}

			_serverAddr.sin_family = AF_INET;
			_serverAddr.sin_port = htons(re.port);

			inet_pton(AF_INET, re.ip, &_serverAddr.sin_addr.s_addr);

			LIB_LOGGER(info, netcode) << "client connected to port: " << re.port;

			return true;
		}

		bool Available() override
		{
			sockaddr_in clientAddr{};

			char temp[1];
			socklen_t clientLen = sizeof(clientAddr);
			ssize_t res = recvfrom(_sockfd, temp, 1, MSG_PEEK, (sockaddr*)&clientAddr, &clientLen);
			if (res < 0)
			{
				return errno == EWOULDBLOCK || errno == EAGAIN ? false : true;
			}
			return true;
		}

		void Listen(int maxListener) override
		{
			sockaddr_in clientAddr{};

			if (Available())
			{
				socklen_t clientLen = sizeof(clientAddr);
				ssize_t recvLen = recvfrom(_sockfd, _buffer, sizeof(_buffer), 0, (sockaddr*)&clientAddr, &clientLen);
				if (recvLen < 0)
				{
					LIB_LOGGER(info, netcode) << "recvfrom failed";
					return;
				}

				char ipStr[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, &(clientAddr.sin_addr), ipStr, INET_ADDRSTRLEN);
				int port = ntohs(clientAddr.sin_port);

				LIB_LOGGER(info, netcode) << "message from: " << ipStr << ":" << port << ", received: " << recvLen;

				Send(clientAddr);
			}
		}

		void SendToServer() override
		{
			std::string reply = "ping!";
			sendto(_sockfd, reply.c_str(), reply.length(), 0, (sockaddr*)&_serverAddr, sizeof(_serverAddr));
		}

	private:
		void Send(const sockaddr_in& clientAddr)
		{
			std::string reply = "pong!";
			sendto(_sockfd, reply.c_str(), reply.length(), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
		}

		bool _success{ false };

		int _sockfd{ -1 };

		sockaddr_in _serverAddr{};

		char _buffer[65536]{};

	};
}

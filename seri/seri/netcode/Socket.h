#pragma once

#include <memory>

namespace seri::netcode
{
	enum SocketType
	{
		udp,
		tcp,
	};

	struct RemoteEndpoint
	{
		const char* ip;
		unsigned int port;
	};

	class Socket
	{
	public:
		virtual ~Socket() = default;

		virtual bool Bind(RemoteEndpoint re) = 0;

		virtual bool Connect(RemoteEndpoint re) = 0;

		virtual bool Available() = 0;

		virtual void Listen(int maxListener = 1) = 0;

		virtual void SendToServer() = 0;

		static std::unique_ptr<Socket> Create(SocketType st);

	};
}

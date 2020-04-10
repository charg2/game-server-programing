#pragma once

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>
#include <cstdint>

#pragma comment(lib, "ws2_32")
// inetNtop () �Լ��� ���ڿ��� �ٲ���. �߰� ����.
// �����ڿ� default param  true, false�� �ְ� 
// true ������ false string"127.0.0.1" �̷���.

class SocketAddress
{
public:
	SocketAddress(uint32_t address, uint16_t port);
	SocketAddress(std::string&& address, uint16_t port);
	SocketAddress(std::wstring&& address, uint16_t port);
	SocketAddress(const char* address, uint16_t port);
	SocketAddress(const wchar_t* address, uint16_t port);
	SocketAddress(const sockaddr& inSockAddr);

	sockaddr* get_as_sockaddr();
	sockaddr_in* get_as_aockaddrin();

	constexpr size_t size() const noexcept
	{
		return sizeof(SocketAddress);
	}
private:

	sockaddr sock_addr;
};



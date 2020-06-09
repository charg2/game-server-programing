#pragma once

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>
#include <cstdint>

#pragma comment(lib, "ws2_32")
// inetNtop () 함수로 문자열로 바꿔줌. 추가 ㄱㄱ.
// 생성자에 default param  true, false로 넣고 
// true 도메인 false string"127.0.0.1" 이런식.
// sockaddr 래퍼

class SocketAddress
{
public:
	SocketAddress(uint32_t address, uint16_t port)
	{
		get_as_aockaddrin()->sin_family = AF_INET;
		get_as_aockaddrin()->sin_port = htons(port);

		get_as_aockaddrin()->sin_addr.s_addr = htonl(address);
	}

	SocketAddress(std::string&& address, uint16_t port)
	{
		get_as_aockaddrin()->sin_family = AF_INET;
		get_as_aockaddrin()->sin_port = htons(port);

		InetPtonA(AF_INET, address.c_str(), &get_as_aockaddrin()->sin_addr);
	}

	SocketAddress(std::wstring&& address, uint16_t port)
	{
		get_as_aockaddrin()->sin_family = AF_INET;
		get_as_aockaddrin()->sin_port = htons(port);

		InetPtonW(AF_INET, address.c_str(), &get_as_aockaddrin()->sin_addr);
	}

	SocketAddress(const char* address, uint16_t port)
	{
		get_as_aockaddrin()->sin_family = AF_INET;
		get_as_aockaddrin()->sin_port = htons(port);
		InetPtonA(AF_INET, address, &get_as_aockaddrin()->sin_addr);
	}

	SocketAddress(const wchar_t* address, uint16_t port)
	{
		get_as_aockaddrin()->sin_family = AF_INET;
		get_as_aockaddrin()->sin_port = htons(port);

		InetPtonW(AF_INET, address, &get_as_aockaddrin()->sin_addr);
	}

	//SocketAddress(in_addr address, uint16_t port)
	//{
	//	//getAsSockAddrIn()->sin_family = AF_INET;
	//	//getAsSockAddrIn()->sin_port = htons(port);

	//	//InetPtonA(AF_INET, inet_ntoa(address), &getAsSockAddrIn()->sin_addr);
	//}

	SocketAddress(const sockaddr& inSockAddr)
	{
		std::memmove(&sock_addr, &inSockAddr, sizeof(sockaddr));
	}

	sockaddr* get_as_sockaddr()
	{
		return &sock_addr;
	}

	sockaddr_in* get_as_aockaddrin()
	{
		return reinterpret_cast<sockaddr_in*>(&sock_addr);
	}

	constexpr size_t size() const noexcept
	{
		return sizeof(SocketAddress);
	}
private:
	sockaddr sock_addr;
};



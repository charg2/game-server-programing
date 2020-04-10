#pragma once

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>
#include <cstdint>

#pragma comment(lib, "ws2_32")
// inetNtop () 함수로 문자열로 바꿔줌. 추가 ㄱㄱ.
// 생성자에 default param  true, false로 넣고 
// true 도메인 false string"127.0.0.1" 이런식.

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



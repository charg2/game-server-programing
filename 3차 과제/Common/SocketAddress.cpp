
#include "SocketAddress.h"
#include <cstdint>

SocketAddress::SocketAddress(uint32_t address, uint16_t port)
{
	get_as_aockaddrin()->sin_family = AF_INET;
	get_as_aockaddrin()->sin_port = htons(port);

	get_as_aockaddrin()->sin_addr.s_addr = htonl(address);
}

SocketAddress::SocketAddress(std::string&& address, uint16_t port)
{
	get_as_aockaddrin()->sin_family = AF_INET;
	get_as_aockaddrin()->sin_port = htons(port);

	InetPtonA(AF_INET, address.c_str(), &get_as_aockaddrin()->sin_addr);
}

SocketAddress::SocketAddress(std::wstring&& address, uint16_t port)
{
	get_as_aockaddrin()->sin_family = AF_INET;
	get_as_aockaddrin()->sin_port = htons(port);

	InetPtonW(AF_INET, address.c_str(), &get_as_aockaddrin()->sin_addr);
}

SocketAddress::SocketAddress(const char* address, uint16_t port)
{
	get_as_aockaddrin()->sin_family = AF_INET;
	get_as_aockaddrin()->sin_port = htons(port);
	InetPtonA(AF_INET, address, &get_as_aockaddrin()->sin_addr);
}

SocketAddress::SocketAddress(const wchar_t* address, uint16_t port)
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

SocketAddress::SocketAddress(const sockaddr& inSockAddr)
{
	std::memmove(&sock_addr, &inSockAddr, sizeof(sockaddr));
}

//constexpr size_t SocketAddress::size()
//{
//	return sizeof(SocketAddress);
//}

sockaddr* SocketAddress::get_as_sockaddr()
{
	return &sock_addr;
}

sockaddr_in* SocketAddress::get_as_aockaddrin()
{
	return reinterpret_cast<sockaddr_in*>(&sock_addr);
}






//#include <string>
//#include <iostream>
//
//void main()
//{
//	std::string  str		{ "0.0.0.0" };
//	std::wstring w_str		{ L"127.0.0.3" };
//	
//	SOCKADDR_IN tt{};
//	tt.sin_family = AF_INET;
//	tt.sin_addr.s_addr = htons(INADDR_ANY);
//	tt.sin_port = htons(1234);
//
//	SocketAddress temp(str.c_str(), 1234);
//	SocketAddress w_temp(w_str.c_str(), 4321);
//
//	std::cout << temp.size() << std::endl;
//	std::cout << temp.get_as_sockaddr() << std::endl;
//
//}

#pragma once



using namespace c2::enumeration;
enum IoType
{
	IO_RECV,
	IO_SEND,
	IO_DISCONNECT,
	IO_ACCEPT,
};
struct IoContext
{
	OVERLAPPED		overalpped;
	const IoType	io_type;
	void*			session;
};
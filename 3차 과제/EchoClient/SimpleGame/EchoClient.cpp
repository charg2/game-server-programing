#include <iostream>

#include "EchoSession.h"
#include "EchoClient.h"


#include "../../Common/SocketAddress.h"

std::unique_ptr<EchoClient> g_echo_client	{ nullptr };

EchoClient::EchoClient() : OuterClient{}
{
}

EchoClient::~EchoClient()
{}

void EchoClient::on_update()
{
}

bool EchoClient::on_initialize_after_init()
{
	return true;
}

bool EchoClient::on_initialize_before_init()
{
	return true;
}

bool EchoClient::on_finalize_after_fin()
{
	return true;
}

bool EchoClient::on_finalize_before_fin()
{
	return true;
}

void EchoClient::on_connect()
{
	hi();
}

void EchoClient::on_disconnect()
{}

ClientSession* EchoClient::create_session()
{
	EchoSession* session = new EchoSession{};
	session->bind_client(this);
	return session;
}

EchoSession* EchoClient::get_session()
{
	return (EchoSession*)session;
}

uint64_t EchoClient::get_session_id()
{
	return session->get_session_id();
}

bool EchoClient::try_get_other_player(uint64_t session_id, OtherPlayer*& out_other)
{
	auto result_iterator = others.find(session_id);
	if (others.end() != result_iterator)
	{
		out_other = result_iterator->second;
		return true;
	}
	else
	{
		return false;
	}
}

void EchoClient::hi()
{
	HiRequest request;

	this->session->pre_send(&request);
}

void EchoClient::connect_using_input()
{
	c2::util::assert_if_false(nullptr != this->session);

	std::cout << "IP를 입력해주세요! : ";

	std::string ip;
	std::cin >> ip;

	SocketAddress sock_address{ ip.c_str(), c2::constant::server_port };

	session->bind_client(this);

	session->connect_using_sockaddress(sock_address);

	on_connect();
}



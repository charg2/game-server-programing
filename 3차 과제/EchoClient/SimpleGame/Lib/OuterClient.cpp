
#include "ClientSession.h"
#include "OuterClient.h"
#include "../../../Common/SocketAddress.h"
#include "../../../Common/exception.h"



OuterClient::OuterClient()
	:session{},
	version{},
	last_error{},
	custom_error_code{ },
	is_running{}
{
	c2::util::assert_if_false(init_network());
}

OuterClient::~OuterClient()
{
	::WSACleanup();
}

void OuterClient::connect()
{
	c2::util::assert_if_false(nullptr != this->session);

	SocketAddress sock_address{ c2::constant::server_ip.data(), c2::constant::server_port };

	session->bind_client(this);

	session->connect_using_sockaddress(sock_address);

	on_connect();
}

void OuterClient::disconnect()
{
	session->disconnect();

	on_disconnect();
}

bool OuterClient::on_initialize_after_init()
{
	return true;
}

bool OuterClient::on_initialize_before_init()
{
	return true;
}

void OuterClient::on_connect()
{}

void OuterClient::on_disconnect()
{}

bool OuterClient::on_finalize_after_fin()
{
	return true;
}

bool OuterClient::on_finalize_before_fin()
{
	return true;
}

void OuterClient::initialize()
{
	c2::util::assert_if_false(on_initialize_before_init());

	this->session = create_session();

	session->init(true);

	c2::util::assert_if_false(on_initialize_after_init());
}

void OuterClient::finalize()
{
	c2::util::assert_if_false(on_finalize_before_fin());

	WSACleanup();

	c2::util::assert_if_false(on_finalize_after_fin());
}

void OuterClient::update()
{
	update_network_io();

	on_update();
}

void OuterClient::on_update()
{
}

const std::string_view& OuterClient::get_version() const
{
	return c2::constant::version.data();
}

void OuterClient::set_error_code(c2::enumeration::ErrorCode error_code)
{
	this->custom_error_code = error_code;
}

ClientSession* OuterClient::create_session()
{
	return nullptr;
}

void OuterClient::update_network_io()
{
	/*const timeval	time_val{ 0, 0 };
	FD_SET			read_set{ 1, this->session->get_socket(), };

	SOCKET client_sock = this->session->get_socket();

	size_t	sock_count = 1;

	int ret_count = select(0, &read_set, nullptr, nullptr, &time_val);
	if (1 == ret_count)
	{
		if (FD_ISSET(client_sock, &read_set))
		{
			session->recv_payload();
		}
	}
	else if (ret_count == 0)
	{
		return;
	}
	else
	{
		c2::util::crash_assert();
	}*/

	session->recv_payload();
	session->send_payload();

	return;
}

bool OuterClient::init_network()
{
	WSAData wsa;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsa))
	{
		c2::util::crash_assert();
		printf("Client::init() WSAStartup() failure");
		this->last_error = static_cast<size_t>(c2::enumeration::ErrorCode::Error_winsock_lib_init_failure);
		return false;
	}

	return true;
}

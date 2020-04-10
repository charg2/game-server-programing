#pragma once



struct Environment
{
	Environment();

	bool	enable_nagle_opt;
	bool	enable_keep_alive;

	char*	server_ip;
	short	server_port;
};
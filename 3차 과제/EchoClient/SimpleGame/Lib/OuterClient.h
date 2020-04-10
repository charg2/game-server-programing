#pragma once

#include <unordered_map>
#include "pre_compile.h"
#include "configuration.h"


class ClientSession;

// non - blocking client
class OuterClient
{
public:
	OuterClient();
	virtual ~OuterClient();

	void connect();
	void disconnect();
	void initialize();
	void finalize();

	void update();
	virtual void on_update();

	virtual bool on_initialize_after_init();
	virtual bool on_initialize_before_init();

	virtual bool on_finalize_after_fin();
	virtual bool on_finalize_before_fin();

	virtual void on_connect();
	virtual void on_disconnect();

	const std::string_view& get_version() const;

	void set_error_code(c2::enumeration::ErrorCode error_code);

	virtual ClientSession* create_session();


private:
	void update_network_io();
	bool init_network();

protected:
	ClientSession*									session;
	//std::unordered_map<uint64_t, OtherPlayer*>		other_map;
	std::string_view								version;
	uint32_t										last_error;
	c2::enumeration::ErrorCode						custom_error_code;// tls·Î?
	bool											is_running;
};


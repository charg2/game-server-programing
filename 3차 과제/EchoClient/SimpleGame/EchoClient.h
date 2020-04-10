#pragma once

#include <memory>
#include <unordered_map>
#include "Lib/OuterClient.h"



class OtherPlayer;

class EchoClient : public OuterClient
{
public:
	EchoClient();
	virtual ~EchoClient();

	virtual void on_update() override final;

	virtual bool on_initialize_after_init()  override final;
	virtual bool on_initialize_before_init() override final;

	virtual bool on_finalize_after_fin()  override final;
	virtual bool on_finalize_before_fin() override final;

	virtual void on_connect()    override final;
	virtual void on_disconnect() override final;

	virtual ClientSession* create_session() override final;

	EchoSession*	get_session();
	uint64_t		get_session_id();
	bool			try_get_other_player(uint64_t session_id, OtherPlayer*& out_other);


	void			hi();
	
	void			connect_using_input();

	std::unordered_map<uint64_t, OtherPlayer*> others;

	friend class EchoSession;
};

extern std::unique_ptr<EchoClient> g_echo_client;
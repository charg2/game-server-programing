#pragma once

#include <queue>
#include <vector>
#include <unordered_map>
#include <string_view>
#include <thread>

#include "pre_compile.h"
#include "configuration.h"
#include "Session.h"



using namespace c2;
struct Session;

// ø‹∫Œ ≈ÎΩ≈∏¡.
class OuterServer
{
public:
	OuterServer();
	OuterServer(const OuterServer& server) = delete;
	OuterServer(OuterServer&& server) noexcept = delete;
	virtual ~OuterServer();

	bool initialize();
	void finalize();

	void disconnect_session(Session* session);// , c2::enumeration::DisconnectReason dr);
	void start();

	virtual bool on_initialize_after_init();
	virtual bool on_initialize_before_init();
	virtual void on_connect(Session* session);
	virtual void on_disconnect(Session* session);
	virtual bool on_accept(Session* session);
	virtual void on_recv();
	virtual void on_finalize_after_fin();
	virtual void on_finalize_before_fin();
	virtual void on_update();
	virtual Session* create_session() = 0;

	const std::string_view& get_version() const;
	c2::enumeration::ErrorCode			get_error_code();
	void								set_error_code(c2::enumeration::ErrorCode err_code);
	size_t								get_ccu();
	std::vector<Session*>&				get_sessions();

private:
	void start_thread();
	bool init_network();

protected:
	void io_thread_function();
	void accept_thread_function();
 
	static void run(OuterServer* server, c2::enumeration::ThreadType thread_type);

protected:
	std::unordered_map<size_t, Session*>	session_map;	// for io thread
	std::vector<Session*>					sessions;		// for io thread
	std::queue<Session*>					new_sessions; 
	std::thread*							threads[c2::enumeration::TT_MAX];
	SRWLOCK									srw_lock;
	SOCKET									listen_socket;
	size_t									generated_session_id;
	std::string_view						version;
	uint32_t								last_error;
	c2::enumeration::ErrorCode				custom_error_code;
	bool									is_running;
};


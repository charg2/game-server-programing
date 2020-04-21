#pragma once
#include <vector>
#include "pre_compile.h"


struct ThreadInfo
{
	OuterServer*				server;
	c2::enumeration::ThreadType thread_tye;
	size_t						index;
};

class Session;
class OuterServer
{
public:
	OuterServer();
	OuterServer(const OuterServer& server) = delete;
	OuterServer(OuterServer&& server) noexcept = delete;
	virtual ~OuterServer();

	void load_config_using_json(const wchar_t* file_name);
	bool initialize();
	bool init_network();
	bool init_sessions();
	bool init_threads();
	bool init_system();
	
	void finalize();

	void start();
	void stop();

	void disconnect_session(Session* session);

	virtual void		on_connect(Session* session);
	virtual void		on_disconnect(Session* session);
	virtual bool		on_accept(Session* session) = 0;
	virtual	Session*	create_sessions(size_t n);
	void				disconnect(uint64_t session_id);

	virtual void		on_wake_io_thread();
	virtual void		on_sleep_io_thread();
	void				disconnect_after_sending(uint64_t session_id);

	Session*							acquire_session_ownership(int64_t session_id);
	void								release_session_ownership(int64_t session_id);


	const wchar_t*						get_version() const;
	const c2::enumeration::ErrorCode	get_custom_last_error() const;
	constexpr size_t					get_ccu() const;

	void								set_custom_last_error(c2::enumeration::ErrorCode err_code);

	size_t								get_toatl_recv_bytes();
	size_t								get_toatl_sent_bytes();

protected:
	void					accepter_procedure(uint64_t idx);
	void					io_service_procedure(uint64_t idx);
	virtual void			custom_precedure(uint64_t idx) = 0;
	static uint32_t WINAPI 	start_thread(LPVOID param);

protected:
	// virtual function table								// 								
	SOCKET						listen_sock;				// 8
	HANDLE						accepter;					// 8
	HANDLE						completion_port;			// 8

	HANDLE*						io_handler;					// 8
	c2::enumeration::ErrorCode	custom_server_error;		// 8  user
	c2::enumeration::ErrorCode	custom_kernel_error;		// 8  kernel
	uint16_t					max_listening_count;		// 2


	Session*					sessions[5000];				// 40000
	/*HANDLE						session_heap;				//8 */
	uint64_t					concurrent_connected_user;	// 8 


	size_t						concurrent_thread_count;	// 8
	wchar_t						ip[16];						// 32
	wchar_t						version[16];				// 32
	bool						nagle_opt;					// 1
	bool						keep_alive_opt;				// 1
	uint16_t					capacity;					// 2
	uint16_t					port;						// 2


	uint64_t*					total_recv_bytes;			// 8
	uint64_t*					total_recv_count;			// 8
	uint64_t*					total_sent_bytes;			// 8
	uint64_t*					total_sent_count;			// 8


private:
	static LPFN_ACCEPTEX		accept_ex;			
	static LPFN_DISCONNECTEX	disconnect_ex;
	static LPFN_CONNECTEX		connect_ex;

	static thread_local size_t	local_storage_accessor;

	friend class Session;
};


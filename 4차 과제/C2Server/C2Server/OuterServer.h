#pragma once

#include <type_traits>
#include "concurrency/ConcurrentStack.h"


#define session_mapping_helper( session_class ) \
static_assert(std::is_base_of<Session, session_class>::value, #session_class ## "is not derived from Session"); \
session_class* session_class##_ptr = (session_class*)HeapAlloc(session_heap, 0, sizeof(session_class) * capacity); \
for (size_t i = 0; i < capacity; ++i) \
{ \
sessions[i] = (Session*)&session_class##_ptr[i]; \
new(sessions[i]) session_class(); \
} \

class Session;
class OuterServer
{
	struct ThreadInfo
	{
		OuterServer*					server;
		c2::enumeration::ThreadType		thread_tye;
		size_t							index;
	};

public:
	OuterServer();
	OuterServer(const OuterServer& server) = delete;
	OuterServer(OuterServer&& server) noexcept = delete;
	virtual ~OuterServer();

	void load_config_using_json(const wchar_t* file_name);
	bool initialize();
	bool init_network_and_system();
	bool init_sessions();
	bool init_threads();
	
	void start();
	void finalize();

	virtual void		on_connect(uint64_t session_id);
	virtual void		on_disconnect(uint64_t session_id);
	virtual bool		on_accept(Session* session);
	virtual void		on_wake_io_thread();
	virtual void		on_sleep_io_thread();
	
	virtual void		on_update();

	virtual void		on_create_sessions(size_t n);
	void				destroy_sessions();

	void				request_disconnection(uint64_t session_id, c2::enumeration::DisconnectReason dr);
	void				release_session(Session* session);

	void				disconnect_after_sending_packet(uint64_t session_id, c2::Packet* out_packet);
	void				send_packet(uint64_t session_id, c2::Packet* out_packet);
	
	Session*							acquire_session_ownership(int64_t session_id);
	void								release_session_ownership(int64_t session_id);


	const wchar_t*						get_version() const;
	const c2::enumeration::ErrorCode	get_os_last_error() const;
	const c2::enumeration::ErrorCode	get_server_last_error() const;
	constexpr size_t					get_ccu() const;

	void								set_custom_last_error(c2::enumeration::ErrorCode err_code);

	void								setup_dump();

	size_t								get_total_recv_bytes();
	size_t								get_total_sent_bytes();
	size_t								get_total_recv_count();
	size_t								get_total_sent_count();

protected:
	void					accepter_procedure(uint64_t idx);
	void					io_service_procedure(uint64_t idx);
	virtual void			custom_precedure(uint64_t idx);
	static uint32_t WINAPI 	start_thread(LPVOID param);


protected:
	// virtual function table								// 								
	SOCKET						listen_sock;				// 8
	HANDLE						accepter;					// 8
	HANDLE						completion_port;			// 8

	HANDLE*						io_handler;					// 8
	c2::enumeration::ErrorCode	custom_last_server_error;	// 8  user
	c2::enumeration::ErrorCode	custom_last_os_error;		// 8  kernel

	Session**					sessions;					// 8
	uint16_t					maximum_listening_count;		// 2
	HANDLE						session_heap;				//8 
	uint16_t					maximum_accpet_count;	// 8 

	size_t						concurrent_thread_count;	// 8
	wchar_t						ip[16];						// 32
	wchar_t						version[16];				// 32
	bool						enable_nagle_opt;					// 1
	bool						enable_keep_alive_opt;				// 1
	uint16_t					capacity;					// 2
	uint16_t					port;						// 2

	c2::concurrency::ConcurrentStack<uint64_t, 5000>	id_pool;

	alignas(c2::constant::CACHE_LINE)	int64_t		total_recv_bytes;			// 8
	alignas(c2::constant::CACHE_LINE)	int64_t		total_recv_count;			// 8
	alignas(c2::constant::CACHE_LINE)	int64_t		total_sent_bytes;			// 8
	alignas(c2::constant::CACHE_LINE)	int64_t		total_sent_count;			// 8
	alignas(c2::constant::CACHE_LINE)	uint64_t	current_accepted_count;			// 8

private:
	static inline LPFN_ACCEPTEX		accept_ex		{};
	static inline LPFN_DISCONNECTEX	disconnect_ex	{};
	static inline LPFN_CONNECTEX	connect_ex		{};

	static inline thread_local size_t	local_storage_accessor{};

	friend class Session;
};


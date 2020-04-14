#pragma once
#include "pre_compile.h"


class Session;
class OuterServer
{
public:
	OuterServer();
	OuterServer(const OuterServer& server) = delete;
	OuterServer(OuterServer&& server) noexcept = delete;
	//virtual ~OuterServer();

	bool initialize();
	bool init_network();
	bool init_sessions();
	
	void finalize();

	void disconnect_session(Session* session);
	void start();

	virtual void		on_connect(Session* session);
	virtual void		on_disconnect(Session* session);
	virtual bool		on_accept(Session* session) = 0;
	virtual	Session*	create_session() = 0;

	const wchar_t*						get_version() const;
	const c2::enumeration::ErrorCode	get_error_code() const;
	constexpr size_t					get_ccu() const;

	void								set_error_code(c2::enumeration::ErrorCode err_code);

protected:
	void			accepter_procedure();
	void			io_service_procedure();
	virtual void	custom_precedure() = 0;

	static void		start_thread(OuterServer* server, c2::enumeration::ThreadType thread_type);

protected:
	// always read / write 
	uint64_t					concurrent_connected_user;	// 
	c2::enumeration::ErrorCode	custom_last_error;			//
	size_t						concurrent_thread_count;	//
	SOCKET						listen_sock;				//
	wchar_t*					version;					//
	wchar_t*					ip;							//
	uint16_t					port;						//
	HANDLE						completion_port;			//

private:
	static LPFN_ACCEPTEX		acceptex;
	static LPFN_DISCONNECTEX	disconnectex;
	static LPFN_CONNECTEX		connectex;

	friend class Session;
};

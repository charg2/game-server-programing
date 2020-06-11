#pragma once
#include "pre_compile.h"
//#include <sqltypes.h>
//#include <sqlext.h>

struct SQL_CONN
{
	SQL_CONN() : sql_hdbc{}, sql_hstmt{}, using_now{}
	{
	}

	SQLHDBC		sql_hdbc ;
	SQLHSTMT	sql_hstmt ;
	bool		using_now ;
};

class DbHelper
{
public:
	DbHelper();
	~DbHelper();

	static bool initialize(const wchar_t* connetion_info_str, int worker_thread_count);
	static void finalize();

	bool execute(const wchar_t* sqlstmt);
	bool fetch_row();


	/// �Ʒ��� DbHelper ��ü�� �����ϰ� ����ؾ� ��
	bool bind_param_int(int* param);
	bool bind_param_float(float* param);
	bool bind_param_bool(bool* param);
	bool bind_param_wstr(const wchar_t* text);
	bool bind_param_str(const char* text);

	/// FetchRow�� �����Ͱ� �ִٸ� �ݵ�� ���ʴ�� ȣ���ؾ���
	void bind_result_column_int(int* r);
	void bind_result_column_float(float* r);
	void bind_result_column_bool(bool* r);
	void bind_result_column_wstr(wchar_t* text, size_t count);
	void bind_result_column_str(char* text, size_t count);

	void print_sql_stmt_error();

private:
	SQLHSTMT	current_sql_hstmt = nullptr;

	SQLSMALLINT	current_result_col;
	SQLSMALLINT	current_bind_param;
	
	static inline SQL_CONN*		sql_connection_pool{}; ///< ��Ŀ���������ŭ
	static inline SQLHENV		sql_henv{};
	static inline int			db_reader_thread_count{};
};


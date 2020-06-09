#pragma once
#include "pre_compile.h"
//#include <sqltypes.h>
//#include <sqlext.h>

struct SQL_CONN
{
	SQLHDBC		mSqlHdbc;
	SQLHSTMT	mSqlHstmt;
	bool		mUsingNow;
};

class DBHelper
{
public:
	DBHelper();
	~DBHelper();

	static bool initialize(const wchar_t* connInfoStr, int workerThreadCount);
	static void finalize();

	bool execute(const wchar_t* sqlstmt);
	bool fetch_row();


	/// 아래는 DbHelper 객체를 선언하고 사용해야 함

	bool bind_param_int(int* param);
	bool bind_param_float(float* param);
	bool bind_param_bool(bool* param);
	bool bind_param_text(const wchar_t* text);

	/// FetchRow시 데이터가 있다면 반드시 차례대로 호출해야함
	void bind_result_column_int(int* r);
	void bind_result_column_float(float* r);
	void bind_result_column_bool(bool* r);
	void bind_result_column_text(wchar_t* text, size_t count);

	void print_sql_stmt_error();

private:
	SQLHSTMT	current_sql_hstmt = nullptr;

	SQLSMALLINT	current_result_col;
	SQLSMALLINT	current_bind_param;

	static inline SQLHENV		sql_henv;
	static inline SQL_CONN*		sql_conn_pool; ///< 워커스레드수만큼
	static inline int			db_worker_thread_count;
};


#include "DBHelper.h"
#include "util/exception.h"

DbHelper::DbHelper()
{
	c2::util::assert_if_false(sql_connection_pool[c2::local::db_thread_id].using_now == false);

	current_sql_hstmt = sql_connection_pool[c2::local::db_thread_id].sql_hstmt;
	current_result_col = 1;
	current_bind_param = 1;

	c2::util::assert_if_false(current_sql_hstmt != nullptr);

	sql_connection_pool[c2::local::db_thread_id].using_now = true;
}

DbHelper::~DbHelper()
{
	SQLFreeStmt(current_sql_hstmt, SQL_UNBIND);
	SQLFreeStmt(current_sql_hstmt, SQL_RESET_PARAMS);
	SQLFreeStmt(current_sql_hstmt, SQL_CLOSE);

	sql_connection_pool[c2::local::db_thread_id].using_now = false;
}

bool DbHelper::initialize(const wchar_t* connetion_info_str, int reader_thread_count)
{
	sql_connection_pool = new SQL_CONN[reader_thread_count];
	db_reader_thread_count = reader_thread_count;

	//환경 핸들 할당
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sql_henv))
	{
		printf_s("DbHelper Initialize SQLAllocHandle failed\n");
		return false;
	}

	//환경 속성 초기화
	if (SQL_SUCCESS != SQLSetEnvAttr(sql_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER))
	{
		printf_s("DbHelper Initialize SQLSetEnvAttr failed\n");
		return false;
	}


	//SQLAllocHandle을 이용하여 SQL_CONN의 SqlHdbc 핸들 사용가능하도록 처리
	//Connection 핸들
		/// 스레드별로 SQL connection을 풀링하는 방식
	for (int i = 0; i < db_reader_thread_count; ++i)
	{
		if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, sql_henv, &sql_connection_pool[i].sql_hdbc))
		{
			printf_s("DbHelper Initialize SQLAllocHandle SQL_HANDLE_DBC failed\n");
			return false;
		}

		SQLSMALLINT resultLen = 0;

		//SQLDriverConnect를 이용하여 SQL서버에 연결하고 그 핸들을 SQL_CONN의 sql_hdbc에 할당
		SQLRETURN ret = SQLDriverConnect(
			sql_connection_pool[i].sql_hdbc,
			NULL,
			(SQLWCHAR*)connetion_info_str,
			(SQLSMALLINT)wcslen(connetion_info_str),
			NULL,
			0,
			&resultLen,
			SQL_DRIVER_NOPROMPT
		);


		if (SQL_SUCCESS != ret && SQL_SUCCESS_WITH_INFO != ret)
		{
			SQLWCHAR sqlState[1024]{};
			SQLINTEGER nativeError{};
			SQLWCHAR msgText[1024]{};
			SQLSMALLINT textLen{};

			SQLGetDiagRec(SQL_HANDLE_DBC, sql_connection_pool[i].sql_hdbc, 1, sqlState, &nativeError, msgText, 1024, &textLen);

			wprintf_s(L"DbHelper Initialize SQLDriverConnect failed: %s \n", msgText);

			return false;
		}


		if (SQL_SUCCESS != SQLAllocHandle(
			SQL_HANDLE_STMT,
			sql_connection_pool[i].sql_hdbc,
			&sql_connection_pool[i].sql_hstmt))
		{
			printf_s("DbHelper Initialize SQLAllocHandle SQL_HANDLE_STMT failed\n");
			return false;
		}
	}
	return true;
}

void DbHelper::finalize()
{
	for (int i = 0; i < db_reader_thread_count; ++i)
	{
		SQL_CONN* currConn = &sql_connection_pool[i];
		if (currConn->sql_hstmt)
			SQLFreeHandle(SQL_HANDLE_STMT, currConn->sql_hstmt);

		if (currConn->sql_hdbc)
			SQLFreeHandle(SQL_HANDLE_DBC, currConn->sql_hdbc);
	}

	delete[] sql_connection_pool;
}

bool DbHelper::execute(const wchar_t* sqlstmt)
{
	// current_sql_hstmt핸들 사용하여 sqlstmt를 수행.  
	SQLRETURN ret = SQLExecDirect(
		current_sql_hstmt,
		(SQLWCHAR*)sqlstmt,
		SQL_NTS  //NULL로 끝나는 문자열
	);

	if (SQL_SUCCESS != ret && SQL_SUCCESS_WITH_INFO != ret)
	{
		print_sql_stmt_error();
		return false;
	}

	return true;
}

bool DbHelper::fetch_row()
{
	//DONE: current_sql_hstmt가 들고 있는 내용 fetch
	SQLRETURN ret = SQLFetch(current_sql_hstmt);

	if (SQL_SUCCESS != ret && SQL_SUCCESS_WITH_INFO != ret)
	{
		if (SQL_NO_DATA != ret)
		{
			print_sql_stmt_error();
		}

		return false;
	}

	return true;
}

bool DbHelper::bind_param_int(int* param)
{
	SQLRETURN ret = SQLBindParameter(
		current_sql_hstmt,
		current_bind_param++,
		SQL_PARAM_INPUT,
		SQL_C_LONG,
		SQL_INTEGER,
		10,
		0,
		param,
		0,
		NULL
	);

	if (SQL_SUCCESS != ret && SQL_SUCCESS_WITH_INFO != ret)
	{
		print_sql_stmt_error();
		return false;
	}

	return true;
}

bool DbHelper::bind_param_float(float* param)
{
	SQLRETURN ret = SQLBindParameter(current_sql_hstmt, current_bind_param++, SQL_PARAM_INPUT,
		SQL_C_FLOAT, SQL_REAL, 15, 0, param, 0, NULL);

	if (SQL_SUCCESS != ret && SQL_SUCCESS_WITH_INFO != ret)
	{
		print_sql_stmt_error();
		return false;
	}

	return true;
}

bool DbHelper::bind_param_bool(bool* param)
{
	SQLRETURN ret = SQLBindParameter(
		current_sql_hstmt,
		current_bind_param++,
		SQL_PARAM_INPUT,
		SQL_C_TINYINT,
		SQL_TINYINT,
		3,
		0,
		param,
		0,
		NULL
	);

	if (SQL_SUCCESS != ret && SQL_SUCCESS_WITH_INFO != ret)
	{
		print_sql_stmt_error();
		return false;
	}

	return true;
}

bool DbHelper::bind_param_wstr(const wchar_t* text)
{
	//DONE: 유니코드 문자열 바인딩
	size_t len = wcslen(text);
	SQLRETURN ret = SQLBindParameter(
		current_sql_hstmt,
		current_bind_param++,
		SQL_PARAM_INPUT,
		SQL_C_WCHAR, SQL_WVARCHAR,
		len, 0, (SQLPOINTER)text, 0, NULL
	);


	if (SQL_SUCCESS != ret && SQL_SUCCESS_WITH_INFO != ret)
	{
		print_sql_stmt_error();
		return false;
	}

	return true;
}

bool DbHelper::bind_param_str(const char* text)
{
	size_t len = strlen(text);
	SQLRETURN ret = SQLBindParameter(
		current_sql_hstmt,
		current_bind_param++,
		SQL_PARAM_INPUT,
		SQL_C_CHAR, SQL_VARCHAR,
		len, 0, (SQLPOINTER)text, 0, NULL
	);


	if (SQL_SUCCESS != ret && SQL_SUCCESS_WITH_INFO != ret)
	{
		print_sql_stmt_error();
		return false;
	}

	return true;
}

void DbHelper::bind_result_column_int(int* r)
{
	SQLLEN len = 0;
	SQLRETURN ret = SQLBindCol(current_sql_hstmt, current_result_col++, SQL_C_LONG, r, 4, &len);

	if (SQL_SUCCESS != ret && SQL_SUCCESS_WITH_INFO != ret)
	{
		print_sql_stmt_error();
	}
}

void DbHelper::bind_result_column_float(float* r)
{
	SQLLEN len = 0;
	SQLRETURN ret = SQLBindCol(
		current_sql_hstmt,
		current_result_col++,
		SQL_C_FLOAT, //CType
		r, //dstPointer
		sizeof(float), //BufferSize
		&len //결과 length
	);
	if (SQL_SUCCESS != ret && SQL_SUCCESS_WITH_INFO != ret)
	{
		print_sql_stmt_error();
	}
}

void DbHelper::bind_result_column_bool(bool* r)
{
	SQLLEN len = 0;
	SQLRETURN ret = SQLBindCol(current_sql_hstmt, current_result_col++, SQL_C_TINYINT, r, sizeof(bool), &len);
	if (SQL_SUCCESS != ret && SQL_SUCCESS_WITH_INFO != ret)
	{
		print_sql_stmt_error();
	}
}

void DbHelper::bind_result_column_wstr(wchar_t* text, size_t count)
{
	SQLLEN len = 0;
	//DONE: wchar_t*형 결과 컬럼 바인딩
	SQLRETURN ret = SQLBindCol(
		current_sql_hstmt,
		current_result_col++,
		SQL_C_WCHAR, //CType
		text, //dstPointer
		count * sizeof(wchar_t), //BufferSize  
		&len //결과 length
	);

	if (SQL_SUCCESS != ret && SQL_SUCCESS_WITH_INFO != ret)
	{
		print_sql_stmt_error();
	}
}

void DbHelper::bind_result_column_str(char* text, size_t count)
{
	SQLLEN len = 0;
	//DONE: wchar_t*형 결과 컬럼 바인딩
	SQLRETURN ret = SQLBindCol(
		current_sql_hstmt,
		current_result_col++,
		SQL_C_CHAR, //CType
		text, //dstPointer
		count * sizeof(char), //BufferSize  
		&len //결과 length
	);

	if (SQL_SUCCESS != ret && SQL_SUCCESS_WITH_INFO != ret)
	{
		print_sql_stmt_error();
	}
}

void DbHelper::print_sql_stmt_error()
{
	SQLWCHAR		sql_state[1024];
	SQLINTEGER		native_error{};
	SQLWCHAR		msg_text[1024]{};
	SQLSMALLINT		text_length{};

	SQLGetDiagRec(SQL_HANDLE_STMT, current_sql_hstmt, 1, sql_state, &native_error, msg_text, 1024, &text_length);

	wprintf_s(L"DbHelper SQL Statement Error: %ls \n", msg_text);
}

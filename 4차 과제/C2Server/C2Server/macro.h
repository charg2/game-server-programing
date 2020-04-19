#pragma once

#define get_session_stamp(num)				( num >> 16 )
#define get_session_index(num)				( num >> 16 )
#define set_session_id(num, session_idx)	( ( num << 16 ) | (uint16_t)session_idx )

#define release_mode
#ifdef release_mode
	#define debug_code( a )			/##/ { (a); }
	#define debug_console( a )		/##/ { (a); }
#else
	#define debug_code( a )			{ (a); }
	#define debug_console( a )		{ wprintf(/##/a); }
#endif



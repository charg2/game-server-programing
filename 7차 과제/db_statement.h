#pragma once

const inline wchar_t* sql_create_actor			{ L"{ call dbo.create_user (?) }" };
const inline wchar_t* sql_load_actor			{ L"{ call dbo.load_user (?) }" };
const inline wchar_t* sql_update_actor_position	{ L"{ call dbo.update_user_position (?, ?, ?) }" };


#pragma once

const inline wchar_t* sql_create_actor			{ L"{ call dbo.create_actor (?) }" };
const inline wchar_t* sql_load_actor			{ L"{ call dbo.load_actor (?) }" };
const inline wchar_t* sql_update_actor_position	{ L"{ call dbo.update_actor_position (?, ?, ?) }" };


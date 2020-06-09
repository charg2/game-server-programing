#include "main.h"
#include "MMOServer.h"

/*
서버
- IOCP
- 맵
- 몬스터
- NPC
- 길찾기
- DB
- SCRIPT
- 채팅
- 장애물
- 버프
- FSM
- 1초에 한칸

클라
- HP 레벨 표시
- 메시지 창 ( 채팅 가능 )
- 알림 창
*/

#include <memory>

void main()
{
	g_server = new MMOServer();

	g_server->setup_dump();

	g_server->load_config_using_json(L"config.json");

	g_server->init_npcs();

	g_server->initialize();

	g_server->start();

	g_server->finalize();
}
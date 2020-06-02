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
	std::unique_ptr<MMOServer> server{ std::make_unique<MMOServer>() };
	
	server->setup_dump();

	server->load_config_using_json(L"config.json");

	server->init_npcs();

	server->initialize();

	server->start();
	
	server->finalize();
}
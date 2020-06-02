#include "main.h"
#include "MMOServer.h"

/*
����
- IOCP
- ��
- ����
- NPC
- ��ã��
- DB
- SCRIPT
- ä��
- ��ֹ�
- ����
- FSM
- 1�ʿ� ��ĭ

Ŭ��
- HP ���� ǥ��
- �޽��� â ( ä�� ���� )
- �˸� â
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
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
	g_server = new MMOServer();

	g_server->setup_dump();

	g_server->load_config_using_json(L"config.json");

	g_server->init_npcs();

	g_server->initialize();

	g_server->start();

	g_server->finalize();
}
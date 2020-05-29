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
void main()
{
	MMOServer server;
	
	server.setup_dump();

	server.load_config_using_json(L"config.json");

	server.init_simulator();

	server.initialize();

	server.start();
	
	server.finalize();
}
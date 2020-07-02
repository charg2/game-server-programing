#include "main.h"
#include "MMOServer.h"
#include "DBHelper.h"
#include "contents_enviroment.h"

/*
서버
- IOCP
- 맵 800 x 800 	시야 15 x 15 window 20 x 20 
- ID를 갖고 로그인.
- 캐릭터
	경험치와 레벨 HP 을 갖고 있음 
	
	Player 
	
	레벨업, 레벨... 주위에서 알필요는 없다.
	본인한테만 유니 캐스트.

	1레벨 경험치 100, 2레벨 요구 경험치 200, 3레벨 400, 4레벨 800… 

	5초마다 10%의 HP회복 
	타이머 이벤트로 계속 걸어놓음.

	-HP 0이 되면 경험치 반으로 깍은 후 HP회복 돼서 시작위치로
	주변 모든 플레이어에게 Remove Obj 전한 후...
	타이버 이벤트 걸어 놓고... 
	머 이미 죽은 상태면 이상한 패킷들은 무시해야지; 

	status 변경은 읽을떄 atomic 하게 암튼... 하자;

	모든 정보 DB저장
	경험치 
	스텟 
	레벨 
	머머머머 
	아이템. 

	화면 상단에 HP와 레벨 표시
	

	NPC가 무기를 준다.는 어떤가?


― 몬스터 
• 레벨, HP, 이름 
• 종류별로 다른 아이콘으로 화면에 표시 
• Type 
	– Peace: : 때리기 전에는 가만히 
	– Agro : 근처에 10x10 영역에 접근하면 쫓아 오기 

• 이동 
– 고정 : 자기 자리에 가만히 
– 로밍 : 원래 위치에서 20 x 20 공간을 자유로이 이동 

– 장애물은 A* 길찾기로 피한다. 
	• 무찔렀을 때의 경험치 – 레벨 * 5 

– Agro 몬스터 2배, 로밍 몬스터 2배 
	• 배치 좌표, 이름, TYPE, Level등 모든 정보 Script로 저장 
	• 죽은 후 30초 후 부활

- 전투
	몬스터는 처음 인식한 공격 대상을 계속 공격 
	• A키를 누르면 주위 4방향 몬스터 동시 공격 
	• 공격속도는 1초에 한번 
	• 메시지 창에 메시지 표시 
	– “용사가 몬스터 A를 때려서 10의 데미지를 입혔습니다.” 
	– “몬스터A의 공격으로 15의 데미지를 입었습니다.” 
	– “몬스터 A를 무찔러서 250의 경험치를 얻었습니다.” 

― 이동 
	• 1초에 한 칸 이동 


― 장애물 & 몬스터 배치 & 밸런스 
	• 자율적으로 조정 
	• 제일 재미있게 배치한 팀에게 10% 가산점


- NPC
- 길찾기
- DB
- SCRIPT
- 채팅
- 장애물
- 버프
- FSM
- 1초에 한칸

성능 - 동접 7천명 

- 클라이언트
메시지 창 ( 채팅 가능 )

클라
- HP 레벨 표시
- 메시지 창 ( 채팅 가능 )
- 알림 창
*/

#include <memory>

using namespace c2;
#include "DBManager.h"
#include "PathFinder.h"

#include <iostream>
int main()
{
	setlocale(LC_ALL, "");

	g_server = new MMOServer();

	g_server->setup_dump();

	g_server->load_config_using_json(L"config.json");
	
	printf("config loading ok\n");

	if (false == PathFinder::InitAstarMap((BYTE*)c2::global::obstacle_table, c2::global::obstacle_table_width, c2::global::obstacle_table_height))
		return -3;

	g_db_manager = new DBManager();

	//if( false == g_db_manager->initialize() )
	//	return -4; 

	g_server->init_npcs();

	g_server->initialize();

	//g_db_manager->bind_server_completion_port(g_server->get_completion_port());

	g_server->start();

	g_server->finalize();
}
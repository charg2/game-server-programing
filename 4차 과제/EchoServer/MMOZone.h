#pragma once

class MMOZone
{
public:
	MMOZone();
	~MMOZone();

	void update();
	void broadcast();

	// MMOSector  sectors[800][800];
	// 업데이트를 전역으로 관리할려면 여기서 
	// 플레이어들을 
	
private:
	/// dispatched_players[];
	//  ushort;  

};


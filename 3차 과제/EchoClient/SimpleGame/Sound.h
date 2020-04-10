#pragma once

#include <map>
#include "./Dependencies/IrrKlang.h"
#pragma comment(lib, "irrKlang.lib") 

using namespace irrklang;

class Sound
{
public:
	Sound();
	~Sound();

	// BG SOUND 메인 스레드 상에서 돌아감.
	// stop이 가능
	int CreateBGSound(char* filePath);
	void DeleteBGSound(int index);
	void PlayBGSound(int index, bool bLoop, float volume); 
	void StopBGSound(int index);

	// 새로운 스레드를 만들어서 그 스레드에서 사운드를 실행 시키기 떄문에 사운드가 가능함..
	// stop이 불가능
	// stop이 필요 없는 간단한 소리의 겨우 이걸 사용.
	int CreateShortSound(char* filePath);
	void DeleteShortSound(int index);
	void PlayShortSound(int index, bool bLoop, float volume);		

private:
	ISoundEngine*					m_engine = NULL;

	std::map<int, ISound*>			m_bgSoundList;
	std::map<int, ISoundSource*>	m_shortSoundList;
};


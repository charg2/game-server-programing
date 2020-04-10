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

	// BG SOUND ���� ������ �󿡼� ���ư�.
	// stop�� ����
	int CreateBGSound(char* filePath);
	void DeleteBGSound(int index);
	void PlayBGSound(int index, bool bLoop, float volume); 
	void StopBGSound(int index);

	// ���ο� �����带 ���� �� �����忡�� ���带 ���� ��Ű�� ������ ���尡 ������..
	// stop�� �Ұ���
	// stop�� �ʿ� ���� ������ �Ҹ��� �ܿ� �̰� ���.
	int CreateShortSound(char* filePath);
	void DeleteShortSound(int index);
	void PlayShortSound(int index, bool bLoop, float volume);		

private:
	ISoundEngine*					m_engine = NULL;

	std::map<int, ISound*>			m_bgSoundList;
	std::map<int, ISoundSource*>	m_shortSoundList;
};


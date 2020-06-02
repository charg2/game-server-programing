#include "KeyManager.h"
#include <Windows.h>


KeyManager::KeyManager() : is_key_down { }, is_key_up { }
{
}

KeyManager::~KeyManager()
{
}


// 한번만 눌리는지...
bool KeyManager::key_down( int virtualKey )
{
	if (GetAsyncKeyState(virtualKey) & 0x8000)	// 눌린 상태에서 
	{
		if (false == is_key_down[virtualKey])	// 기존에 눌르지 않앗다면?
		{
			is_key_down[virtualKey] = true;		// 누름 처리.
			return true;
		}
		// 눌린 상태에선 끝남.
	}
	else										// 안눌려 있다면
	{											
		is_key_down[virtualKey] = false; 
	}

	return false;
}


// 한번 눌럿다가 띄어지는지 체크...
bool KeyManager::key_up( int virtualKey )
{
	if (GetAsyncKeyState(virtualKey) & 0x8000)
	{
		is_key_up[virtualKey] = true;
	}
	else
	{
		// 눌려진 순간 true가 되므로 true일때만 진입
		if (true == is_key_up[virtualKey])
		{
			// 키 상태를 false 로 만들어 재진입 불가.
			is_key_up[virtualKey] = false;
			
			return true;
		}
	}
	
	return false;
}

bool KeyManager::toggle_key( int virtualKey )
{
	if ( GetKeyState (virtualKey) & 0x0001 )
		return true;

	return false;
}

bool KeyManager::key_stay( int virtualKey )
{
	if (GetAsyncKeyState(virtualKey) & 0x8000)
		return true;

	return false;
}

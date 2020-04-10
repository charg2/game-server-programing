#include "stdafx.h"
#include "KeyManager.h"
#include <Windows.h>

std::unique_ptr<KeyInput> g_key_input	{ nullptr };

KeyInput::KeyInput() : isKeyDown { }, isKeyUp { }
{
}

KeyInput::~KeyInput()
{
}


// 한번만 눌리는지...
bool KeyInput::keyDown( int virtualKey )
{
	if (GetAsyncKeyState(virtualKey) & 0x8000)	// 눌린 상태에서 
	{
		if (false == isKeyDown[virtualKey])		// 기존에 눌르지 않앗다면?
		{
			isKeyDown[virtualKey] = true;		// 누름 처리.
			return true;
		}
		// 눌린 상태에선 끝남.
	}
	else										// 안눌려 있다면
	{											
		isKeyDown[virtualKey] = false; 
	}

	return false;
}


// 한번 눌럿다가 띄어지는지 체크...
bool KeyInput::keyUp( int virtualKey )
{
	if (GetAsyncKeyState(virtualKey) & 0x8000)
	{
		isKeyUp[virtualKey] = true;
	}
	else
	{
		// 눌려진 순간 true가 되므로 true일때만 진입
		if (true == isKeyUp[virtualKey])
		{
			// 키 상태를 false 로 만들어 재진입 불가.
			isKeyUp[virtualKey] = false;
			
			return true;
		}
	}
	
	return false;
}

bool KeyInput::toggleKey( int virtualKey )
{
	if ( GetKeyState (virtualKey) & 0x0001 )
		return true;

	return false;
}

bool KeyInput::keyStay( int virtualKey )
{
	if (GetAsyncKeyState(virtualKey) & 0x8000)
		return true;

	return false;
}

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


// �ѹ��� ��������...
bool KeyInput::keyDown( int virtualKey )
{
	if (GetAsyncKeyState(virtualKey) & 0x8000)	// ���� ���¿��� 
	{
		if (false == isKeyDown[virtualKey])		// ������ ������ �ʾѴٸ�?
		{
			isKeyDown[virtualKey] = true;		// ���� ó��.
			return true;
		}
		// ���� ���¿��� ����.
	}
	else										// �ȴ��� �ִٸ�
	{											
		isKeyDown[virtualKey] = false; 
	}

	return false;
}


// �ѹ� �����ٰ� ��������� üũ...
bool KeyInput::keyUp( int virtualKey )
{
	if (GetAsyncKeyState(virtualKey) & 0x8000)
	{
		isKeyUp[virtualKey] = true;
	}
	else
	{
		// ������ ���� true�� �ǹǷ� true�϶��� ����
		if (true == isKeyUp[virtualKey])
		{
			// Ű ���¸� false �� ����� ������ �Ұ�.
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

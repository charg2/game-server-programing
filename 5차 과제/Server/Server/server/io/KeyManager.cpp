#include "KeyManager.h"
#include <Windows.h>


KeyManager::KeyManager() : is_key_down { }, is_key_up { }
{
}

KeyManager::~KeyManager()
{
}


// �ѹ��� ��������...
bool KeyManager::key_down( int virtualKey )
{
	if (GetAsyncKeyState(virtualKey) & 0x8000)	// ���� ���¿��� 
	{
		if (false == is_key_down[virtualKey])	// ������ ������ �ʾѴٸ�?
		{
			is_key_down[virtualKey] = true;		// ���� ó��.
			return true;
		}
		// ���� ���¿��� ����.
	}
	else										// �ȴ��� �ִٸ�
	{											
		is_key_down[virtualKey] = false; 
	}

	return false;
}


// �ѹ� �����ٰ� ��������� üũ...
bool KeyManager::key_up( int virtualKey )
{
	if (GetAsyncKeyState(virtualKey) & 0x8000)
	{
		is_key_up[virtualKey] = true;
	}
	else
	{
		// ������ ���� true�� �ǹǷ� true�϶��� ����
		if (true == is_key_up[virtualKey])
		{
			// Ű ���¸� false �� ����� ������ �Ұ�.
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

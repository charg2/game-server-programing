#ifndef KEY_MANAGER_H
#define KEY_MANAGER_H

#include <memory>

constexpr int kKeyMax = 256;


enum VirtualKey
{
	LeftArrow = 0x25,
	UpArrow = 0x26,
	RightArrow,
	DownArrow,
	ESC = 0x18,
	A = 0x41, 
	B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
};

class KeyInput
{
public:
	KeyInput();
	~KeyInput();

	bool keyDown( int virtualKey );	// 키가 내려가는지.
	bool keyUp( int virtualKey );		// 누른 키가 올라가는지.
	bool toggleKey( int virtualKey );	// 토글 키 체크
	bool keyStay( int virtualKey ); // 현재 키가 계속 눌리는지;

private:
	bool isKeyDown[kKeyMax];
	bool isKeyUp[kKeyMax];
};

extern std::unique_ptr<KeyInput> g_key_input;

#endif 
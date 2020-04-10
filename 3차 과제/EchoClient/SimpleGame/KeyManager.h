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

	bool keyDown( int virtualKey );	// Ű�� ����������.
	bool keyUp( int virtualKey );		// ���� Ű�� �ö󰡴���.
	bool toggleKey( int virtualKey );	// ��� Ű üũ
	bool keyStay( int virtualKey ); // ���� Ű�� ��� ��������;

private:
	bool isKeyDown[kKeyMax];
	bool isKeyUp[kKeyMax];
};

extern std::unique_ptr<KeyInput> g_key_input;

#endif 
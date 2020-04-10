#ifndef KEY_MANAGER_H
#define KEY_MANAGER_H

#include <memory>

constexpr size_t kKeyMax = 256;


enum VirtualKey
{
	LeftArrow = 0x25,
	UpArrow = 0x26,
	RightArrow,
	DownArrow,
	A = 0x41, 
	B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
};

class KeyInput
{
public:
	KeyInput();
	~KeyInput();

	bool key_down( int virtualKey );	// Ű�� ����������.
	bool key_up( int virtualKey );		// ���� Ű�� �ö󰡴���.
	bool toggle_key( int virtualKey );	// ��� Ű üũ
	bool key_stay( int virtualKey ); // ���� Ű�� ��� ��������;

private:
	bool is_key_down[kKeyMax];
	bool is_key_up[kKeyMax];
};

extern std::unique_ptr<KeyInput> g_key_input;

#endif 
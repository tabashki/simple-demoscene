#pragma once

#include <Windows.h>

struct Window
{
	HWND handle;

public:
	bool process_messages();
	
#ifdef _DEBUG
	~Window();
#endif // _DEBUG

	static Window* create(HINSTANCE hInst);

private:
	static LRESULT WINAPI proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
};

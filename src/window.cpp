#include "window.h"
#include "utility.h"
#include "memory.h"

#include <cassert>
#include <cstdlib>

const char* window_name = "!";
constexpr int window_width = 1280;
constexpr int window_height = 720;

ATOM wndclass_atom = 0;
bool close_requested = false;

bool Window::process_messages()
{
	MSG msg;

	while (PeekMessage(&msg, handle, 0, 0, PM_REMOVE))
	{
		DispatchMessage(&msg);
	}

	return !close_requested;
}

#ifdef _DEBUG
Window::~Window()
{
	ASSERT(handle);
	DestroyWindow(handle);
}
#endif // _DEBUG

Window* Window::create(HINSTANCE hInstance)
{
	ASSERT(wndclass_atom == 0);

	WNDCLASSA wndclass = { };
	wndclass.lpfnWndProc = &Window::proc;
	wndclass.hInstance = hInstance;
	wndclass.lpszClassName = window_name;

	wndclass_atom = RegisterClassA(&wndclass);
	ASSERT(wndclass_atom != 0);

	Window* win = mem_alloc<Window>();

	constexpr int window_style = WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU; // Add WS_THICKFRAME to allow resizing

	// Adjustment to make sure that the client (drawable) area is exactly the desired width & height
	RECT win_rect = { 0, 0, window_width, window_height };
	AdjustWindowRect(&win_rect, window_style, false);

	win->handle = CreateWindowA(window_name, window_name, window_style,
		CW_USEDEFAULT, CW_USEDEFAULT,		// Window position x, y
		win_rect.right - win_rect.left,		// Window width
		win_rect.bottom - win_rect.top,		// Window height
		NULL, NULL, hInstance, NULL
	);
	ASSERT(win->handle != NULL);

	return win;
}

LRESULT WINAPI Window::proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_CLOSE:
	case WM_QUIT:
		close_requested = true;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

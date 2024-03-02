#include "utility.h"
#include "window.h"
#include "renderer.h"
#include "memory.h"
#include "shaders.h"
#include "timer.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR pCmdLine, int nCmdShow)
{
	Window* window = Window::create(hInstance);
	Renderer* renderer = Renderer::create(*window);

	Timer timer;
	timer.start();

	bool run = true;
	while (run)
	{
		run = window->process_messages();
		run &= renderer->render_frame(timer.elapsed_sec());
	}

#ifdef _DEBUG
	mem_delete(renderer);
	mem_delete(window);
#endif // !_DEBUG

	ExitProcess(0);
}

#ifdef CRINKLED
extern "C" IMAGE_DOS_HEADER __ImageBase;
#define THIS_HINSTANCE() ((HINSTANCE)&__ImageBase)

void WinMainCRTStartup()
{
	WinMain(THIS_HINSTANCE(), NULL, NULL, SW_SHOWDEFAULT);
}
#endif // CRINKLED

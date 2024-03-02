#pragma once

#include "window.h"

#include <d3d11.h>

using CS = ID3D11ComputeShader;
using VS = ID3D11VertexShader;
using PS = ID3D11PixelShader;

struct Renderer
{
	UINT frame_index;
	UINT viewport_width;
	UINT viewport_height;
	IDXGISwapChain* swapchain;
	ID3D11Device* device;
	ID3D11DeviceContext* context;

public:
	void begin_frame(float time);
	void end_frame();

	CS* create_compute_shader(const BYTE* bytecode, size_t size);

	void dispatch_threadgroups(CS* cs, UINT x, UINT y, UINT z = 1);
	void dispatch_fullscreen(CS* cs);

	bool render_frame(float time);

#ifdef _DEBUG
	~Renderer();
#endif // _DEBUG

private:
	void setup_frame_consts(D3D11_VIEWPORT& viewport, float time);

public:
	static Renderer* create(Window& win);
};


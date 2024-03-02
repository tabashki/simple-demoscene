#include "renderer.h"
#include "utility.h"
#include "math.h"
#include "memory.h"
#include "shaders.h"

#include <DirectXMath.h>

constexpr DXGI_FORMAT SWAPCHAIN_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
constexpr UINT SWAPCHAIN_BUFFERS = 3;

struct FRAME_CONSTANTS
{
	UINT screenWidth;
	UINT screenHeight;

	float invAspectRatio;
	float time;
};

static_assert(sizeof(FRAME_CONSTANTS) % 16 == 0, "Constant buffer must be padded to a size multiple of 16");

// -------------------------------------------------------------------------- //

void Renderer::begin_frame(float time)
{
	ID3D11Texture2D* backbuffer = nullptr;
	swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));
	ASSERT(backbuffer);

	D3D11_TEXTURE2D_DESC backbuffer_desc;
	backbuffer->GetDesc(&backbuffer_desc);
	viewport_width = backbuffer_desc.Width;
	viewport_height = backbuffer_desc.Height;

	D3D11_VIEWPORT viewport = {};
	viewport.Width = float(viewport_width);
	viewport.Height = float(viewport_height);
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;

	ID3D11UnorderedAccessView* uav = nullptr;
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
	uav_desc.Format = SWAPCHAIN_FORMAT;
	uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	device->CreateUnorderedAccessView(backbuffer, &uav_desc, &uav);

	context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
	backbuffer->Release();
	uav->Release();

	setup_frame_consts(viewport, time);
}

void Renderer::end_frame()
{
	swapchain->Present(1, 0);
	frame_index++;
}

CS* Renderer::create_compute_shader(const BYTE* bytecode, size_t size)
{
	CS* compute_shader = nullptr;
	device->CreateComputeShader(bytecode, size, nullptr, &compute_shader);
	return compute_shader;
}

void Renderer::dispatch_threadgroups(CS* cs, UINT x, UINT y, UINT z)
{
	context->CSSetShader(cs, nullptr, 0);
	context->Dispatch(x, y, z);
}

void Renderer::dispatch_fullscreen(CS* cs)
{
	// NOTE: assumes a compute shader with a [8, 8, 1] thread group size
	const UINT threadgroups_x = divide_round_up(viewport_width, 8u);
	const UINT threadgroups_y = divide_round_up(viewport_height, 8u);

	dispatch_threadgroups(cs, threadgroups_x, threadgroups_y);
}

bool Renderer::render_frame(float time)
{
	begin_frame(time);
	dispatch_fullscreen(cs_scene_render);
	end_frame();

	return true;
}

void Renderer::setup_frame_consts(D3D11_VIEWPORT& viewport, float time)
{
	// For calculating the perspective projection matrix
	const float aspect_ratio = viewport.Width / viewport.Height;
	const float z_near = 0.00001f;
	const float z_far = 1.f;
	const float z_range = z_far - z_near;

	const float y_scale = 1.f; // cot(fov_y / 2)
	const float x_scale = y_scale / aspect_ratio;

	FRAME_CONSTANTS frame_consts;
	frame_consts.time = time;
	frame_consts.invAspectRatio = 1.f / aspect_ratio;
	frame_consts.screenWidth = viewport_width;
	frame_consts.screenHeight = viewport_height;

	D3D11_BUFFER_DESC cbuf_desc = {};
	cbuf_desc.ByteWidth = sizeof(frame_consts);
	cbuf_desc.Usage = D3D11_USAGE_DYNAMIC;
	cbuf_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbuf_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA cbuf_data = {};
	cbuf_data.pSysMem = &frame_consts;

	ID3D11Buffer* frame_cbuf = nullptr;
	device->CreateBuffer(&cbuf_desc, &cbuf_data, &frame_cbuf);

	context->CSSetConstantBuffers(0, 1, &frame_cbuf);
	frame_cbuf->Release();
}

#ifdef _DEBUG
Renderer::~Renderer()
{
	context->Flush();
	shaders_destroy();

	context->Release();
	swapchain->Release();
	device->Release();
}
#endif // _DEBUG

// -------------------------------------------------------------------------- //

Renderer* Renderer::create(Window& win)
{
	bool fullscreen = false;

	if (MessageBoxA(win.handle, "Fullscreen?", "", MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		fullscreen = true;
	}

	UINT device_flags = 0;
#ifdef _DEBUG
	device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DXGI_SWAP_CHAIN_DESC swapchain_desc = {};
	swapchain_desc.BufferDesc.Format = SWAPCHAIN_FORMAT;
	swapchain_desc.SampleDesc.Count = 1;
	swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	swapchain_desc.BufferCount = SWAPCHAIN_BUFFERS;
	swapchain_desc.OutputWindow = win.handle;
	swapchain_desc.Windowed = !fullscreen;
	swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	const D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* immediate_context = nullptr;
	IDXGISwapChain* swapchain = nullptr;

	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, device_flags, &feature_level, 1,
		D3D11_SDK_VERSION, &swapchain_desc, &swapchain,
		&device, nullptr, &immediate_context);

	ASSERT(swapchain);
	ASSERT(device);
	ASSERT(immediate_context);

	Renderer* rdev = mem_alloc<Renderer>();
	rdev->frame_index = 0;
	rdev->swapchain = swapchain;
	rdev->device = device;
	rdev->context = immediate_context;

	shaders_create(*rdev);

	// Initialize any other global renderer resources here

	return rdev;
}

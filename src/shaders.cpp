#include "shaders.h"
#include "renderer.h"

#include <type_traits>

#include "compiled_shaders/cs_scene_render.h"

CS* cs_scene_render = nullptr;

// -------------------------------------------------------------------------- //

template<typename T>
constexpr bool IsShaderType = (std::is_same_v<T, CS>);

template<typename T, size_t N>
T* create_shader(Renderer& dev, const BYTE(&bytecode)[N])
{
	static_assert(IsShaderType<T>, "Unsupported shader type");

	if constexpr (std::is_same_v<T, CS>)
		return dev.create_compute_shader(bytecode, N);
}

template<typename T>
void destroy_shader(T*& shader)
{
	static_assert(IsShaderType<T>, "Unsupported shader type");

	shader->Release();
	shader = nullptr;
}

void shaders_create(Renderer& dev)
{
	cs_scene_render = create_shader<CS>(dev, cs_scene_render_bytecode);
}

#ifdef _DEBUG
void shaders_destroy()
{
	destroy_shader(cs_scene_render);
}
#endif // _DEBUG

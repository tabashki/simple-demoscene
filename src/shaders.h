#pragma once

#include "renderer.h"

void shaders_create(Renderer& dev);

#ifdef _DEBUG
void shaders_destroy();
#endif // _DEBUG

// -------------------------------------------------------------------------- //

extern CS* cs_scene_render;

#include "common.hlsli"

#define DEBUG_OUTPUT_DIFFUSE 0
#define DEBUG_OUTPUT_NORMAL 0
#define DEBUG_OUTPUT_ITERATIONS 0

static const float3 VIEW_FORWARD = float3(0, 0, 1);
static const float3 VIEW_UP = float3(0, 1, 0);
static const float3 VIEW_RIGHT = float3(1, 0, 0);

static const float MAX_T = 100;
static const float EPSILON = 0.0001;
static const float NUDGE = 0.002;

static const uint PRIMARY_TRACE_ITERS = 32;
static const uint SHADOW_TRACE_ITERS = 24;

// -------------------------------------------------------------------------- //

struct Ray
{
	float3 origin;
	float3 direction;
};

struct RayHit
{
	float ray_t;
	float3 diffuse;
	float3 normal;
#if DEBUG_OUTPUT_ITERATIONS
	uint iterations;
#endif
};

struct Light
{
	float3 color;
	float3 direction;
};

static const float3 AMBIENT = 0.03;
static const uint LIGHT_COUNT = 2;

static const Light LIGHTS[LIGHT_COUNT] = {
	{ float3(1.0, 0.8, 0.7), normalize(float3( 1, 1, -0.2)) },
	{ float3(0.7, 0.7, 1.0), normalize(float3(-1, 1, -0.2)) },
};

// -------------------------------------------------------------------------- //

float scene_sdf(float3 p)
{
	static const float r = sin(g_Time * 1.5) * 0.5 + 1.5;
	static const float x = sin(g_Time) * r;
	static const float y = cos(g_Time) * r;
	static const float x2 = sin(g_Time + PI_2) * r;
	static const float y2 = cos(g_Time + PI_2) * r;
	static const SDFRoundedBox box1 = { float3(x, y, 2), float3(0.45, 0.45, 0.45), 0.15 };
	static const SDFRoundedBox box2 = { float3(-x, -y, 2), float3(0.3, 0.3, 0.3), 0.02 };
	static const SDFSphere sphere1 = { float3(x2, y2, 2), 0.3 };
	static const SDFSphere sphere2 = { float3(-x2, -y2, 2), 0.6 };

	float d = sdf_eval(box1, p);
	d = min(d, sdf_eval(box2, p));
	d = min(d, sdf_eval(sphere1, p));
	d = min(d, sdf_eval(sphere2, p));

	return d;
}

float3 scene_diffuse(float3 p)
{
	return float3(0.5, 0.5, 0.5);
}

float3 scene_normal(float3 p)
{
	const float2 h = float2(EPSILON, 0);
	return normalize(float3(
		scene_sdf(p + h.xyy) - scene_sdf(p - h.xyy),
		scene_sdf(p + h.yxy) - scene_sdf(p - h.yxy),
		scene_sdf(p + h.yyx) - scene_sdf(p - h.yyx)
	));
}

// -------------------------------------------------------------------------- //

RayHit trace_primary(const in Ray ray)
{
	RayHit hit = (RayHit)0;

	for (uint i = 0; i < PRIMARY_TRACE_ITERS; i++)
	{
#if DEBUG_OUTPUT_ITERATIONS
		hit.iterations = i + 1;
#endif
		const float3 p = ray.origin + ray.direction * hit.ray_t;
		float d = scene_sdf(p);
		hit.ray_t += d;
		if ((d < EPSILON) || (hit.ray_t > MAX_T))
			break;
	}

	if (hit.ray_t < MAX_T)
	{
		const float3 p = ray.origin + ray.direction * hit.ray_t;
		hit.diffuse = scene_diffuse(p);
		hit.normal = scene_normal(p);
	}

	return hit;
}

float trace_shadow(const in Ray ray)
{
	const float k = 10;
	float result = 1e9;
	float t = NUDGE;

	for (uint i = 0; i < SHADOW_TRACE_ITERS; i++)
	{
		float d = scene_sdf(ray.origin + ray.direction * t);
		result = min(result, k * d / t);
		t += d;
		if (d < EPSILON)
			return 0;
		if (t > MAX_T)
			break;
	}

	return saturate(result);
}

#if DEBUG_OUTPUT_ITERATIONS
float3 iteration_heat_color(float x, float range)
{
	x = saturate(x / range);
	const float l = lerp(0, 1, frac(x * 4));
	if (x < 0.25)
		return float3(0, l, 0);
	else if (x < 0.5)
		return float3(l, 1, 0);
	else if (x < 0.75)
		return float3(1, 1-l, 0);
	else
		return float3(1-l, 0, l);
}
#endif

float3 trace_lighting(const in Ray ray)
{
	float3 color = 0;
	
	// Primary ray
	RayHit hit = trace_primary(ray);

	float3 hit_pos = ray.origin + ray.direction * hit.ray_t;
	float3 result = 0;

	if (hit.ray_t < MAX_T)
	{
		result += AMBIENT;
	}

	// Secondary shadowing rays
	for (uint li = 0; li < LIGHT_COUNT; li++)
	{
		Light light = LIGHTS[li];
		Ray secondary = { hit_pos, light.direction };
		// Nudge out shadow ray from surface, avoids self-intersecting
		secondary.origin += hit.normal * NUDGE;

		float shadow = trace_shadow(secondary);
		
		result += shadow * light.color * dot(hit.normal, light.direction);
	}

#if DEBUG_OUTPUT_DIFFUSE
	return hit.diffuse;
#elif DEBUG_OUTPUT_NORMAL
	return hit.normal * 0.5 + 0.5;
#elif DEBUG_OUTPUT_ITERATIONS
	return iteration_heat_color(hit.iterations, PRIMARY_TRACE_ITERS);
#endif

	return result;
}

void primary_ray(uint2 pixel, out Ray ray)
{
	float2 screenPos = float2(pixel) / g_ScreenDims * 2.0 - 1.0;

	// Invert Y for DirectX-style coordinates.
	screenPos.y = -screenPos.y;

	ray.origin = screenPos.x * VIEW_RIGHT
				+ g_InvAspectRatio * screenPos.y * VIEW_UP;

	// Perspective projection
	ray.direction = normalize(ray.origin + VIEW_FORWARD);
}

// -------------------------------------------------------------------------- //

[numthreads(8, 8, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
	Ray ray;
	primary_ray(threadID.xy, ray);

	float3 color = trace_lighting(ray);
	g_BackBuffer[threadID.xy] = float4(color, 1);
}

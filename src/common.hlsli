#ifndef COMMON_H
#define COMMON_H

cbuffer FRAME_CONSTS : register(b0)
{
	uint2 g_ScreenDims;
	float g_InvAspectRatio;
	float g_Time;
}

RWTexture2D<float4> g_BackBuffer : register(u0);

static const float PI = 3.14159265358979323846f;
static const float PI_2 = 1.57079632679489661923f;

// -------------------------------------------------------------------------- //

struct SDFBox
{
	float3 origin;
	float3 bounds;
};

struct SDFRoundedBox
{
	float3 origin;
	float3 bounds;
	float radius;
};

struct SDFSphere
{
	float3 origin;
	float radius;
};

// -------------------------------------------------------------------------- //

float sdf_eval(const in SDFBox b, float3 p)
{
	p -= b.origin;
	float3 q = abs(p) - b.bounds;
	return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float sdf_eval(const in SDFRoundedBox b, float3 p)
{
	p -= b.origin;
	float3 q = abs(p) - b.bounds;
	return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0) - b.radius;
}


float sdf_eval(const in SDFSphere s, float3 p)
{
	p -= s.origin;
	return length(p) - s.radius;
}

// -------------------------------------------------------------------------- //

float rand(float co) { return frac(sin(co * (91.3458)) * 47453.5453); }
float rand(float2 co) { return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453); }
float rand(float3 co) { return rand(co.xy + rand(co.z)); }
float rand(float4 co) { return rand(co.xy + rand(co.zw)); }

uint pcg_hash(uint input)
{
	uint state = input * 747796405u + 2891336453u;
	uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return (word >> 22u) ^ word;
}

#endif // COMMON_H

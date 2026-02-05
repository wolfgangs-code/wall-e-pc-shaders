#include "hlsl_math.h"
#include "scattering.h"


struct VS_Tornado_Input
{
	float4	position	: POSITION;
	float3	normal		: TEXCOORD0;
	float4	UVAlphaAdd	: TEXCOORD1;
};

struct VS_Tornado_Output
{
	float4	position	: POSITION;
	float3	eyevec		: TEXCOORD0;
	float3	lightdir	: TEXCOORD1;
	float3	normal		: TEXCOORD2;
	float2	uv			: TEXCOORD3;
	float3	params		: TEXCOORD4;
	float4	inscatter	: TEXCOORD5;
	float4	extinction	: TEXCOORD6;
};

struct VS_Decal_Output
{
	float4	position	: POSITION;
	float4	normal		: TEXCOORD5;
	float4	color		: COLOR0;
	float2	uv			: TEXCOORD0;
	float3	lightDir	: TEXCOORD1;
	float3	axe0		: TEXCOORD2;
	float3	axe1		: TEXCOORD3;
	float3	axe2		: TEXCOORD4;
};


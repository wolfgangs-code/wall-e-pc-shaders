#ifndef __GROUND_WATER
#define __GROUND_WATER

	#include "hlsl_math.h"

	struct VS_Input {
		float4	position	: POSITION;
		float4	color		: COLOR0;
		float2	uv			: TEXCOORD0;
	};

	struct VS_GroundWaterOutput_Z {
		float4	position	: POSITION;

		half4	diffuse		: COLOR0;
		half3	eyevec		: COLOR1;
		float4	uv			: TEXCOORD0;
#ifdef _X360
		float4	zBuffUV		: TEXCOORD1;
#endif
		float4	lightDir	: TEXCOORD2;

		half3	sunlight	: TEXCOORD3;
		half3	skylight	: TEXCOORD4;

		float4	inscatter	: TEXCOORD5;
		float4	extinction	: TEXCOORD6;

#ifdef	bShadowMap
		float4	shadowtcproj: TEXCOORD7;
#endif
	};

#endif

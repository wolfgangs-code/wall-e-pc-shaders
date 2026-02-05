#ifndef	_PARTICLE_H
#define	_PARTICLE_H

	#include "hlsl_math.h"

	struct VS_Input_Heat {
		float4	position	: POSITION;			//xy pos zw duv
		float	alpha		: POSITION1;		//alpha
	};

	struct VS_Input {
		float4	position	: POSITION;
		float3	normal		: POSITION1;
		float4	color		: POSITION2;
		float2	uv		: POSITION3;
		float	f		: POSITION4;
	};


	struct VS_Smoke_Out {
	float4	position	: POSITION0;
	float4	color		: COLOR0;
	float2	diffuse		: TEXCOORD0;
	float4	fog			: COLOR1;
	float4  shadowtcproj: TEXCOORD2;
	};

	struct VS_SmokeOut {
	float4	position	: POSITION0;
	float4	color		: COLOR0;

	float2	diffuse		: TEXCOORD0;
	float4	sundir		: TEXCOORD1;
	float3	skydir		: TEXCOORD2;
	float4	sunlight	: TEXCOORD3;
	float4	skylight	: TEXCOORD4;

	float3	extinction	: TEXCOORD5;
	float3  inscatter	: TEXCOORD6;
	};



#endif

#ifndef	_GROUND_ELEMENT_H
#define	_GROUND_ELEMENT_H

	#include "hlsl_math.h"

	struct VS_Input {
		float4	position0	:POSITION;
		float4	normal0		:POSITION1;
		float4	color		:POSITION2;
		float4	position1	:POSITION3;
		float4	normal1		:POSITION4;
		float2	t0		:POSITION5;
		float2	t1		:POSITION6;
	};

	struct VS_Input_Shadow {
		float4	position0	:POSITION;
		float4	normal0		:POSITION1;
	};

	struct VS_GroundOutput {
		float4	position	: POSITION;
		float4	color		: COLOR0;
		float3	eyevec		: COLOR1;
		float3	sundir		: TEXCOORD0;
		float3	skydir		: TEXCOORD1;
		float3	sunlight	: TEXCOORD2;
		float3	skylight	: TEXCOORD3;
		
		float4	inscatter	: TEXCOORD4;
		float4	extinction	: TEXCOORD5;
		#ifdef	bShadowMap
		float4  shadowtcproj	: TEXCOORD7;
		#endif
	};
	float2	GetDiffuseUV(in VS_GroundOutput i)	
	{
		 float2 uv;
		 uv.x=i.inscatter.w;
		 uv.y=i.extinction.w;
		 return	uv;
	}
	void	SetDiffuseUV(inout VS_GroundOutput o,in const float u,in const float v)	
	{
		o.inscatter.w=u;
		o.extinction.w=v;
	}

#endif

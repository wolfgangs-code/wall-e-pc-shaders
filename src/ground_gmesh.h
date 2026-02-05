#ifndef	_GROUND_GMESH_H
#define	_GROUND_GMESH_H

	#include "hlsl_math.h"

	// Piquets, poteaux, fils electriques...

	struct VS_Input {
		float4	position	:POSITION;
		float4	normal		:NORMAL;
		float4	color		:COLOR0;
		float2	uv		:TEXCOORD0;
	};

	struct VS_Input_Shadow {
		float4	position0	:POSITION;
		float4	normal		:NORMAL;
	};

	struct VS_GroundOutput {
		float4	position	: POSITION;
		half4	color		: COLOR0;
		half3	normal		: TEXCOORD0;
		half3	sundir		: TEXCOORD1;
		half3	skydir		: TEXCOORD2;
		half3	sunlight	: TEXCOORD3;
		half3	skylight	: TEXCOORD4;
		float4	inscatter	: TEXCOORD5;
		float4	extinction	: TEXCOORD6;
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

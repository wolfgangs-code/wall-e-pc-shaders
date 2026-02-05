#ifndef	_GROUND_ROTSHAPE_H
#define	_GROUND_ROTSHAPE_H

	#include "hlsl_math.h"

	struct VS_Input {
		float4	position	:POSITION;
		float4	normal		:NORMAL;
		float4	color		:COLOR0;
		float4	t02			:POSITION1;
		float2	t1			:POSITION2;
	};

	struct VS_Input_Shadow {
		float4	position	:POSITION;
		float4	t02		:POSITION1;
	};
	
	struct VS_GroundRotShapeOutput {
		float4	position	: POSITION;
		half4	color		: COLOR0;
		half4	normal		: COLOR1;
		half4	sundir		: TEXCOORD0;
		half3	skydir		: TEXCOORD1;
		half3	sunlight	: TEXCOORD2;
		half3	skylight	: TEXCOORD3;

		float4	inscatter	: TEXCOORD4;
		float4	extinction	: TEXCOORD5;
		half3	eyevec		: TEXCOORD6;
		#ifdef	bShadowMap
		float4  shadowtcproj	: TEXCOORD7;
		#endif
	};
	float2	GetDiffuseUV(in VS_GroundRotShapeOutput i)	
	{
		 float2 uv;
		 uv.x=i.inscatter.w;
		 uv.y=i.extinction.w;
		 return	uv;
	}
	void	SetDiffuseUV(inout VS_GroundRotShapeOutput o,in const float2 uv)	
	{
		o.inscatter.w=uv.x;
		o.extinction.w=uv.y;
	}
	void	SetOcclusion(inout VS_GroundRotShapeOutput o,in const float v)	
	{
		o.sundir.w=v;
	}
	float	GetOcclusion(in VS_GroundRotShapeOutput i)
	{
		return	i.sundir.w;
	}
	float	GetSpecular(in VS_GroundRotShapeOutput i)	
	{
		return	i.normal.w;
	}
	void	SetSpecular(inout VS_GroundRotShapeOutput o,in const float v)	
	{
		o.normal.w=v;
	}
#endif

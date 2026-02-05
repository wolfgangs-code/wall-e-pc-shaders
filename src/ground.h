#ifndef	_GROUND_H
#define	_GROUND_H

	#include "hlsl_math.h"

	struct VS_Input {
		float4	position0	:POSITION;
		float4	normal0		:POSITION1;
		float4	position1	:POSITION2;
		float4	t0001		:POSITION3;
		float4	color0		:POSITION4;
		float4	normal1		:POSITION5;
		float4	color1		:POSITION6;
		float4	t1011		:POSITION7;
		float4	t1012		:POSITION8;
		float4	t1013		:POSITION9;
	};

	struct VS_Input_Shadow {
		float4	position0	:POSITION;
		float4	normal0		:POSITION1;
		float4	position1	:POSITION2;
		float4	t0001		:POSITION3;
	};
	

	struct VS_GroundOutput {
		float4	position	: POSITION;
		half4	color		: COLOR0;
		half4	eyevec		: TEXCOORD4;
		half4   variables	: TEXCOORD0;
		
		half4	sundir		: TEXCOORD1;
		half4	skydir		: TEXCOORD2;
		half3	omni		: TEXCOORD3;

		float4	inscatter	: TEXCOORD5;
		float4	extinction	: TEXCOORD6;
#ifdef	bShadowMap
		float4  shadowtcproj	: TEXCOORD7;
  #ifdef _X360
		float4 shadowNoiseUv : TEXCOORD9;
  #endif
#endif
	};

// VS INPUT ACCESSORS



// VS OUTPUT PS INPUT ACCESSORS

	float2	GetBumpUV(in VS_GroundOutput i)	
	{
		 float2 uv;
		 uv.x=i.sundir.w;
		 uv.y=i.skydir.w;
		 return	uv;
	}
	void	SetBumpUV(inout VS_GroundOutput o,in const float u,in const float v)	
	{
		o.sundir.w=u;
		o.skydir.w=v;
	}
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
	float	GetSpecular(in VS_GroundOutput i)			{return	i.variables.z;}
	void	SetSpecular(inout VS_GroundOutput o,in const float v)	{o.variables.z=v;}
	void	SetOcclusion(inout VS_GroundOutput o,in const float v)	{o.variables.w=v;}
	float	GetOcclusion(in VS_GroundOutput i)			{return	i.variables.w;}
	float	GetBump1Lerp(in VS_GroundOutput i)			{return	i.variables.x;}
	void	SetBump1Lerp(inout VS_GroundOutput i,in const float v)	{i.variables.x=v;}
	float	GetBump2Lerp(in VS_GroundOutput i)			{return	i.variables.y;}
	void	SetBump2Lerp(inout VS_GroundOutput i,in const float v)	{i.variables.y=v;}
#endif

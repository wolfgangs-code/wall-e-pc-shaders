#ifndef	_SCAT_MESH_H
#define	_SCAT_MESH_H

	#include "hlsl_math.h"
	#include "mesh.h"

	struct VS_ScatOutput {
		float4	position	: POSITION;
		float4	eyevec		: TEXCOORD4;
		
		S_TexSpace			TexSpace;

		float4	omni		: TEXCOORD0;
		float4	inscatter	: TEXCOORD5;
		float4	extinction	: TEXCOORD6;

		#ifdef	bShadowMap
			float4  shadowtcproj	: TEXCOORD7;
			#ifdef _X360
				//float4 shadowNoiseUv : TEXCOORD8;
			#endif
		#endif

	};
	float2	GetDiffuseUV(in VS_ScatOutput i)	
	{
		 float2 uv;
		 uv.x=i.inscatter.w;
		 uv.y=i.extinction.w;
		 return	uv;
	}
	void	SetDiffuseUV(inout VS_ScatOutput o,in const float2 uv)	
	{
		o.inscatter.w=uv.x;
		o.extinction.w=uv.y;
	}
	float	GetDirtiness(in VS_ScatOutput i)						{	return i.eyevec.w;	}
	void	SetDirtiness(inout VS_ScatOutput o,in const float v)	{	o.eyevec.w=v;	}
	float	GetBroken(in VS_ScatOutput i)							{	return i.omni.w;	}
	void	SetBroken(inout VS_ScatOutput o,in const float v)		{	o.omni.w=v;	}
#endif

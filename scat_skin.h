#ifndef	_SCAT_SKIN_H
#define	_SCAT_SKIN_H

	#include "hlsl_math.h"

	struct VS_Input {
		float4	position	: POSITION;
		float4	tangent		: POSITION1;
		float3	normal		: NORMAL;
		float2	uv			: TEXCOORD0;
		float2	luv			: TEXCOORD1;
	};


	struct VS_ScatOutput {
		float4	position	: POSITION;
		half3	eyevec		: TEXCOORD4;

		S_TexSpace			TexSpace;

		half4	inscatter	: TEXCOORD5;
		half4	extinction	: TEXCOORD6;
		
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
	float	GetDirtiness(in VS_ScatOutput i)						{	return 0;	}
	void	SetDirtiness(inout VS_ScatOutput o,in const float v)	{	}
	float	GetBroken(in VS_ScatOutput i)							{	return 0;	}
	void	SetBroken(inout VS_ScatOutput o,in const float v)		{	}
#endif

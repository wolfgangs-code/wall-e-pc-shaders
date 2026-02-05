#ifndef	_MESH_H
#define	_MESH_H


	#include "hlsl_math.h"

	/*
		En cas de Morphing, les shaders utilisent POSITION2 donc ne pas le rajouter dans VS_Input !!
	*/

	struct VS_Input {
		float4	position	: POSITION;
		float4	tangent		: POSITION1;
		float3	normal		: NORMAL;
		float2	uv			: TEXCOORD0;
		float2	luv			: TEXCOORD1;
#ifdef	bGouraud
		float4	color		: COLOR0;
#endif
	};


	struct VS_Input_Shadow {
		float4	position	: POSITION;
		float3	normal		: NORMAL;
		float2	uv			: TEXCOORD0;
		float2	luv			: TEXCOORD1;
	};

	float4	TargetMorph(	in const float4 position,		//Position
							in const float3 delta,			//Delta du Morph
							out float pixelmorph)			//
	{
		//Pour obtenir les valeurs mis dans le Mesh par la structure MorphTargetValue_Z
		//utiliser les defines C_TARGET_MORPH0 a C_TARGET_MORPH8

		float4	newposition=position;
		float	ratio=C_TARGET_MORPH0.x;//sin( C_DTIME  ) * 0.5f + 0.5f;
		newposition.xyz+=ratio*delta;
		pixelmorph=ratio;
		return newposition;
	}

#endif

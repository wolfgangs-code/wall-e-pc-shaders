#ifndef	_ROTSHAPE_H
#define	_ROTSHAPE_H

#include "hlsl_math.h"

	struct VS_Input {
		float4	position	: POSITION;
		float3	delta		: POSITION1;
		float4	color		: COLOR0;
		float2	uv			: TEXCOORD0;
	};

	struct VS_Vertex3D {
		float4	position	: POSITION;		// Vertex pos
		float4	diffuse		: COLOR0;		// Vertex color
		float2	uv			: TEXCOORD0;	// Vertex texture coordinate
	};


#endif

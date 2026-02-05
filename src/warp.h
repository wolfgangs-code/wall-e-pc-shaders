	#include "hlsl_math.h"

	struct VS_SkyOutput {
		float4	position	: POSITION;
		float4  inscatter	: TEXCOORD0;
		float4	extinction	: TEXCOORD1;
		float4	texcoord12	: TEXCOORD2;
		float2  params		: TEXCOORD3;
	};
     
	struct VS_SkysunOutput {
		float4	position	: POSITION;
		float3	inscatter	: TEXCOORD0;
		float3	extinction	: TEXCOORD1;
		float3	eye			: TEXCOORD2;
		float2	texCoord	: TEXCOORD3;
	};
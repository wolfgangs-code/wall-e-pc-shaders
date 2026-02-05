	#include "hlsl_math.h"

	struct VS_SkyOutput {
		float4	position	: POSITION;
		float4  inscatter	: TEXCOORD0;
		float4	extinction	: TEXCOORD1;
		half4	texcoord12	: TEXCOORD2;
		float2  params		: TEXCOORD3;
	};
     
	struct VS_SkysunOutput {
		float4	position	: POSITION;
		float3	inscatter	: TEXCOORD0;
		float3	extinction	: TEXCOORD1;
		half3	eye			: TEXCOORD2;
		half2	texCoord	: TEXCOORD3;
	};
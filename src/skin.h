#ifndef	_SKIN_H
#define	_SKIN_H

	#include "hlsl_math.h"

	struct VS_Input_1Blend {
		float4	position	: POSITION;
		float3	normal		: NORMAL;
		float2	uv			: TEXCOORD0;
		float4	tangent		: POSITION1;
		int		matid		: BLENDINDICES;
		float	blend		: POSITION2;
	};

	struct VS_Input_4Blend {
		float4	position	: POSITION;
		float3	normal		: NORMAL;
		float2	uv			: TEXCOORD0;
		float4	tangent		: POSITION1;
		int4	matids		: BLENDINDICES;
		float4	blends		: POSITION2;
	};

	struct VS_Output_Blended
	{
		float4	position;		//blended
		float3	normal;
		float3	tangent;		
		float3	utangent;		//unpacked tangent
		float	handedness;
	};

	const float4	cache_mat0[3]		: register(vs, c51 );
	const float3	cache_rot0[3]		: register(vs, c54 );

	float3	BlendPos(const float4 p,int matid,float blend)
	{
		float4x3	m=float4x3(	cache_mat0[matid].x,cache_mat0[matid+1].x,cache_mat0[matid+2].x,
						cache_mat0[matid].y,cache_mat0[matid+1].y,cache_mat0[matid+2].y,
						cache_mat0[matid].z,cache_mat0[matid+1].z,cache_mat0[matid+2].z,
						cache_mat0[matid].w,cache_mat0[matid+1].w,cache_mat0[matid+2].w);
		return	blend*mul(p,m);
	}
	float3	BlendDir(const float3 p,int matid,float blend)
	{
		float3x3	m=float3x3(	cache_rot0[matid].x,cache_rot0[matid+1].x,cache_rot0[matid+2].x,
						cache_rot0[matid].y,cache_rot0[matid+1].y,cache_rot0[matid+2].y,
						cache_rot0[matid].z,cache_rot0[matid+1].z,cache_rot0[matid+2].z);
	
		return	blend*mul(p,m);
	}
	void Blend1(in const VS_Input_1Blend i,in const float4 position,out VS_Output_Blended o)
	{
		o.utangent.xyz	=i.tangent.xyz*C_TWO-C_UNIT;
		o.handedness	=i.tangent.w*C_TWO-C_UNIT;
		float3	normal	=i.normal*C_TWO-C_UNIT;
		o.position.w=C_UNIT;
		int	matid		=i.matid.x;
		o.normal		=BlendDir(normal,		matid,i.blend);
		o.tangent		=BlendDir(o.utangent,	matid,i.blend);
		o.position.xyz	=BlendPos(position,		matid,i.blend);
		o.tangent		=normalize(o.tangent);
		o.normal		=normalize(o.normal);
	}
	void Blend4(in const VS_Input_4Blend i,in const float4 position,out VS_Output_Blended o)
	{
		float3	normal	=i.normal*C_TWO-C_UNIT;
		o.utangent.xyz	=i.tangent.xyz*C_TWO-C_UNIT;
		o.handedness	=i.tangent.w*C_TWO-C_UNIT;
		o.position.w	=C_UNIT;
		int		matid0	=i.matids.x;
		int		matid1	=i.matids.y;
		int		matid2	=i.matids.z;
		int		matid3	=i.matids.w;
#ifdef	_NO_VERTEXCOMP
		float4	blends	=i.blends;
#else
		float4	blends	=i.blends*C_INV256;
#endif
		o.position.xyz	=BlendPos(position,		matid0,blends.x);
		o.normal		=BlendDir(normal,		matid0,blends.x);
		o.tangent		=BlendDir(o.utangent,	matid0,blends.x);
		o.position.xyz	+=BlendPos(position,	matid1,blends.y);
		o.normal		+=BlendDir(normal,		matid1,blends.y);
		o.tangent		+=BlendDir(o.utangent,	matid1,blends.y);
		o.position.xyz	+=BlendPos(position,	matid2,blends.z);
		o.normal		+=BlendDir(normal,		matid2,blends.z);
		o.tangent		+=BlendDir(o.utangent,	matid2,blends.z);
		o.position.xyz	+=BlendPos(position,	matid3,blends.w);
		o.normal		+=BlendDir(normal,		matid3,blends.w);
		o.tangent		+=BlendDir(o.utangent,	matid3,blends.w);
		o.tangent		=normalize(o.tangent);
		o.normal		=normalize(o.normal);
	//	o.position.xyz		=position.xyz;
	}
	void Blend(const VS_Output_Blended b,const float2 inUV,inout VS_Output o)
	{
		BaseMaterial(inUV,inUV,o.TexCoord);
		o.fog		= Fog(b.position);
		float4		wPos;
		o.position = WorldAndProjectVertex(b.position,wPos);
	#ifdef	bShadowMap
		wPos.w=C_UNIT;
		o.shadowtcproj=ProjShadow(wPos);
	#endif

#ifdef bEnvmap
		o.envmap	= Envmap(b.normal,b.position,o.position);
#endif
		
#ifdef	bVLight
		VertexLighting(b.position,b.normal,o);
#else	
		//Binormal * sign
		float3		vBinormal = cross(b.normal,b.tangent)*b.handedness;
		
		//Tangent Space
		float3x3	mLight = LightMat(b.tangent,vBinormal,b.normal);
		OutTexSpace(o.TexSpace,mLight,b.position,b.normal);

		//Lighting Pixel or Vertex
	#ifdef	bSpecular
		o.eyevec	= EYE_LOCAL.xyz-b.position;
	#endif
	#ifdef	bOmni		
		o.omnitcproj= ProjSpotLight0(wPos);
	#endif
#endif
	}

#ifndef	bVLight
	void BlendOmni(const VS_Output_Blended b,const float2 inUV,inout VS_Output_Omni o)
	{
		BaseMaterial(inUV,inUV,o.TexCoord);
		float4		wPos;
		o.position	= WorldAndProjectVertex(b.position,wPos);

		//Binormal * sign
		float3		vBinormal = cross(b.normal,b.tangent)*b.handedness;
		
		//Tangent Space
		float3x3	mLight = LightMat(b.tangent,vBinormal,b.normal);
		OutTexSpace(o.TexSpace,mLight,b.position,b.normal);

		//Lighting Pixel or Vertex
#ifdef	bSpecular
		o.eyevec	= EYE_LOCAL.xyz-b.position;
#endif

#ifdef	bOmni		
		Out_Omnis(o,wPos);
#endif
	}
#endif

#endif

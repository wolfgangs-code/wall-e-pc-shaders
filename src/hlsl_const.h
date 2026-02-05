#ifndef	_HLSL_CONST_H
#define	_HLSL_CONST_H
	
	#include "VS_Const.h"
const float4	c[C_END_CONSTANT]	: register(vs, c0);

	// Pixel Shader Constants
	struct PixelOmni {
		float4		Pos;
		float4		Dir;		//W = OnSpecular			
		float4		Value;
		float4		Color;
	};

	struct PixelMat {
		float4		Diffuse		: register(c0);	//MaterialDiffuse * DLight Color
		float4		Emissive	: register(c1);	//MaterialDiffuse * DLight Ambient + MaterialEmissive .w=opacity
		float4		Specular	: register(c2);	//MaterialSpecular w=Glossiness
	};

	struct DLight {
		float4		Dir			: register(c3);	
		float4		Color		: register(c4);	//.w ?? luminance apres
		float4		Ambient		: register(c5);	//.w On Off Specular
	};
	const PixelMat		gMaterial;
	const DLight		gDLight;
	const PixelOmni	gOmni0;			//Omnis definition
	const PixelOmni	gOmni1;			//Omnis definition
	const PixelOmni	gOmni2;			//Omnis definition
	const PixelOmni	gOmni3;			//Omnis definition
	const PixelOmni	gOmni4;			//Omnis definition
	const float4		gMaterialParams : register(c6);//MtlRefraction.x MtlEnvFactor.y ...
	const float4		gRefraction;	//MtlDiffuse.xyz + MtlEmissive.xyz MtlgRefraction.w
	const float		gDeltaTime;		//Frame delta time
	const float4		gCamera;		//CameraWorld Pos
	const float4		gShadowMap;		//.x=ShadowMapSize .y=1/ShadowMapSize
	const float4		gDSkyColor;
	const float4		gDSunColor;
	const float3		gDSkyAmbient;
	const float3		gDSkyDir;
	const float3		gGouraudDefault;

	//Sample disponible et remappé

	//Stage des matieres
	sampler2D sDiffuse : register(s0);
#ifdef bRadiosity
	sampler2D sRadiosityMap : register(s1);
#endif

#ifdef bEnvmap
	sampler2D sEnvmap : register(s2);
#endif
	sampler2D sSpecular;
	sampler2D sNormal;
	sampler2D sNormalLocal;
	sampler2D sAddNormalLocal;
	sampler2D sOcclusion;
	sampler2D sScreenCopy;
	sampler2D sZBuffer;
	sampler2D sShadowMap;
	samplerCUBE sGlobalCube;
	sampler2D sDiffuseShadowMap;
#ifdef _X360
	sampler3D sShadowNoiseMap;
#endif
	sampler2D sDirt;

	//Extra
	sampler2D sShrinkedScreenCopy;
	sampler2D sRenderTarget;
	sampler2D sBlendScreenMap;

	//Omni Proj Map
	sampler2D sProjSpotMap0;
	sampler2D sProjSpotMap1;
	sampler2D sProjSpotMap2;
	sampler2D sProjSpotMap3;
	sampler2D sProjSpotMap4;

	sampler2D	sBumpMap0;
	sampler2D	sBumpMap1;
	sampler2D	sBumpMap2;

//-----------------------------------------------------------------------------
	struct S_TexSpace {
#ifdef	bNormal
		float4	axe0		: TEXCOORD1;	//XYZ Texture Space W=Local Vertex X
		float4	axe1		: TEXCOORD2;	//XYZ Texture Space W=Local Vertex Y
		float4	axe2		: TEXCOORD3;	//XYZ Texture Space W=Local Vertex Z
#else
	#ifndef	bNormalLocal
		float3	normal		: TEXCOORD1;	//Vertex Normal or Texture is Local NormalMap
	#endif
		float3	pos			: TEXCOORD2;	//Local Vertex
#endif
	};

#ifdef	bVLight	// Vertex lighting (ps1.2)

	struct S_TexCoord {
		float2	diffuse		: TEXCOORD0;
	#ifdef	bRadiosity
		float2	radiosity	: TEXCOORD1;
	#endif
	};

	struct VS_Output {
		float4	position	: POSITION;
		S_TexCoord			TexCoord;
	#ifdef	bEnvmap
		float3  envmap		: TEXCOORD2;
	#endif
		float4  fog			: COLOR1;
		float4  light		: COLOR0;
	#ifdef	bSpecular
		float3  eyevec		: TEXCOORD4;
	#endif
	};
	struct PS_Input
	{
		S_TexCoord			TexCoord;
	#ifdef	bEnvmap
		float3 envmap		: TEXCOORD2;
	#endif
		float4 fog			: COLOR1;
		float4 light			: COLOR0;
	#ifdef	bSpecular
		float3 eyevec		: TEXCOORD4;
	#endif
	};
//-----------------------------------------------------------------------------
#else			// Pixel Lighting (ps2.0)
	struct S_TexCoord {
		float4	diffuse		: TEXCOORD0;
	};

	struct VS_Output {
		float4	position	: POSITION;		//Vertex pos
		S_TexCoord			TexCoord;
		S_TexSpace			TexSpace;
	#ifdef	bEnvmap
		float3  envmap		: TEXCOORD4;	//envmap texcoord or proj position 
	#endif
	#ifdef	bGouraud
		float4  color		: TEXCOORD4;	//color
	#endif
	#ifdef	bShadowMap
		float4  shadowtcproj	: TEXCOORD4;
		#ifdef _X360
			float4 shadowNoiseUv : TEXCOORD8;
		#endif
	#endif
		float4  fog			: TEXCOORD5;	//fog params
	#ifdef	bSpecular
		float3  eyevec		: TEXCOORD6;	//eye vector (unormalized)
	#endif
	#ifdef	bOmni
		float4  omnitcproj	: TEXCOORD7;
	#endif
	#ifdef	bBlendScreen
		float4	projpos		: TEXCOORD7;	//Utilise dans Pawap, pour blender (Lamber) le rendu lors du compute radiosity
	#endif
	};
	struct PS_Input
	{
		S_TexCoord			TexCoord;
		S_TexSpace			TexSpace;
	#ifdef	bEnvmap
		float3  envmap		: TEXCOORD4;	//envmap texcoord or proj position 
	#endif
	#ifdef	bGouraud
		float4  color		: TEXCOORD4;	//color
	#endif
	#ifdef	bShadowMap
		float4  shadowtcproj	: TEXCOORD4; 
		#ifdef _X360
			float4 shadowNoiseUv : TEXCOORD8;
		#endif
	#endif
		float4  fog			: TEXCOORD5;	//fog params
	#ifdef	bSpecular
		float3  eyevec		: TEXCOORD6;	//eye vector (unormalized)
	#endif
	#ifdef	bOmni
		float4  omnitcproj	: TEXCOORD7;
	#endif
	#ifdef	bBlendScreen
		float4	projpos		: TEXCOORD7;	//Utilise dans Pawap, pour blender (Lamber) le rendu lors du compute radiosity
	#endif

	};

				// Omnis*2 Pixel Lighting (ps2.0)
	struct VS_Output_Omni {
		float4	position	: POSITION;		//Vertex pos
		S_TexCoord			TexCoord;
		S_TexSpace			TexSpace;
	#ifdef	bSpecular
		float3  eyevec		: TEXCOORD4;	//eye vector (unormalized)
	#endif
		float4  omni0tcproj	: TEXCOORD5;
		float4  omni1tcproj	: TEXCOORD6;
	};
	struct PS_Input_Omni
	{
		S_TexCoord			TexCoord;
		S_TexSpace			TexSpace;
	#ifdef	bSpecular
		float3  eyevec		: TEXCOORD4;	//eye vector (unormalized)
	#endif
		float4  omni0tcproj	: TEXCOORD5;
		float4  omni1tcproj	: TEXCOORD6;
	};
#endif

// Shadow volumes (stencil)
//-----------------------------------------------------------------------------
struct VS_Output_Volume {
	float4	position	: POSITION;
	float3	color		: COLOR0;
};

//-----------------------------------------------------------------------------
struct VS_Output_Shadow {
	float4	position	: POSITION;
	float4	color		: COLOR0;
};

//-----------------------------------------------------------------------------
struct VS_Output_Ref {
	float4	position	: POSITION;
	float3  light		: COLOR0;		//dlight direction (normalized)
	float4  diffuse		: TEXCOORD0;	//diffuse.xy texcoord + rad
	float2	uv			: TEXCOORD4;	//mesh uv
	float4	projpos		: TEXCOORD1;		//= position
#ifndef	bNormal
	float3  normal		: TEXCOORD2;	//Normal interpolé
#endif
#ifdef	bEnvmap
	float2  envmap		: TEXCOORD3;
#endif
#ifdef	bSpecular
	float3	eyevec		: TEXCOORD6;
#endif
};
struct PS_Input_Ref {
	float4  diffuse		: TEXCOORD0;
	float2	uv			: TEXCOORD4;	//mesh uv
	float3  light		: COLOR0;		//dlight direction (normalized)
	float4	projpos		: TEXCOORD1;
#ifndef	bNormal
	float3  normal		: TEXCOORD2;	//Normal interpolé
#endif
#ifdef	bEnvmap
	float2  envmap		: TEXCOORD3;
#endif
#ifdef	bSpecular
	float3	eyevec		: TEXCOORD6;
#endif
};
//-----------------------------------------------------------------------------
struct VS_Output_Water {
	float4	position		: POSITION;
	float4	fog				: COLOR1;		//fog params
	float4	color			: COLOR0;
	float2	diffuse			: TEXCOORD0;
	float2	radiosity		: TEXCOORD1;
	float2	reflection		: TEXCOORD2;
	float3	fresnel			: TEXCOORD3;
};
struct PS_Input_Water {
	float4	fog				: COLOR1;		//fog params
	float4	color			: COLOR0;
	float2	diffuse			: TEXCOORD0;
	float2	radiosity		: TEXCOORD1;
	float2	reflection		: TEXCOORD2;
	float3	fresnel			: TEXCOORD3;
};

//-----------------------------------------------------------------------------
struct VS_3DIn {
	float4	position : POSITION0; // position (xyz) | fog coeff (w)
	float4	color	 : COLOR0;
	float2	diffuse	 : TEXCOORD0;
};
struct VS_3DOut {
	float4	position	: POSITION0;
	float4	color		: COLOR0;
	float2	diffuse		: TEXCOORD0;
	float4	fog		: COLOR1;
};
struct VS_2DInOut {
	float4	position	: POSITION;
	float4	color		: COLOR0;
	float2	diffuse		: TEXCOORD0;
};
//-----------------------------------------------------------------------------

#endif
#ifndef	_HLSL_MATH_H
#define	_HLSL_MATH_H


	#include "hlsl_const.h"
	#include "hlsl_lighting.h"
	
	// return x*x+y*y+z*z
	float GetNorm2(float4 vec)
	{
		return dot(vec.xyz,vec.xyz);
	}
	// return sqrt(x*x+y*y+z*z)
	float GetNorm(float4 vec)
	{
		return sqrt(dot(vec.xyz,vec.xyz));
	}
	float4x4 GetMatrix( int CST)
	{
		return float4x4(
				c[CST].x,c[CST+1].x,c[CST+2].x,c[CST+3].x,
				c[CST].y,c[CST+1].y,c[CST+2].y,c[CST+3].y,
				c[CST].z,c[CST+1].z,c[CST+2].z,c[CST+3].z,
				c[CST].w,c[CST+1].w,c[CST+2].w,c[CST+3].w);
	}
	float4 ViewVertex(in const float4 inPos)
	{
		return mul(inPos, GetMatrix(C_VIEW_CONTEXT));
	}
	float4 ProjVertex(in const float4 inPos)
	{
		return mul(inPos, GetMatrix(C_PROJ_CONTEXT));
	}

	//Coord World
	float4 WorldToViewVertex(in const float4 wPos)
	{
		return mul(wPos-VIEW_TRANS, GetMatrix(C_VIEW_CONTEXT));
	}
	float4 WorldToProjVertex(in const float4 wPos)
	{
		return	mul(mul(wPos-VIEW_TRANS,GetMatrix(C_VIEW_CONTEXT)),GetMatrix(C_PROJ_CONTEXT));
	}
	//Coord Local
	float4 WorldVertex(in const float4 lPos)
	{
		return	VIEW_TRANS+mul(lPos,GetMatrix(C_WORLD_CONTEXT));
	}
	float4 ProjectVertex(in const float4 lPos)
	{
		float4	cPos=mul(lPos,GetMatrix(C_WORLD_CONTEXT));
		return	mul(mul(cPos,GetMatrix(C_VIEW_CONTEXT)),GetMatrix(C_PROJ_CONTEXT));
	}

	float4 WorldAndProjectVertex(in const float4 lPos,out float4 worldPos)
	{
		float4	cPos=mul(lPos,GetMatrix(C_WORLD_CONTEXT));
		worldPos=cPos+VIEW_TRANS;
		return	mul(mul(cPos,GetMatrix(C_VIEW_CONTEXT)),GetMatrix(C_PROJ_CONTEXT));
	}
	float3 WorldRotate(in const float3 inNormal)
	{
		float3	v;
		v.x=dot(inNormal,c[C_WORLD_CONTEXT].xyz);
		v.y=dot(inNormal,c[C_WORLD_CONTEXT+1].xyz);
		v.z=dot(inNormal,c[C_WORLD_CONTEXT+2].xyz);
		return	v;
	}	
	float4 ProjShadow(in const float4 inPos)
	{
		return mul(inPos-SHADOWMAP_VIEWPOS, GetMatrix(C_SHADOWMAP_CONTEXT));
	}
	float ProjectZ(float z)
	{
		float m_ProjQ = c[C_PROJ_CONTEXT+2].z;
		float neg_m_ProjQN = c[C_PROJ_CONTEXT+2].w;
		return (z*m_ProjQ+neg_m_ProjQN);
	}
	half3 Envmap(in const float3 inNormal,in const float3 inPos,float4 inProjPos)
	{
		float3	t;
		#ifdef	bEnvmapScreen
			t.xy=inProjPos.xy;
			t.z=inProjPos.w;
		#else
			float3	eyevec=EYE_LOCAL.xyz-inPos.xyz;
			eyevec=normalize(eyevec);
			float3	reflect=dot(inNormal,eyevec)*inNormal*C_TWO-eyevec;
			t.xyz = reflect.xzz*C_HALF+C_HALF;
/*			float3	p=WorldRotate(inNormal);
			p = normalize(p);
			t.x=dot(p,c[C_VIEW_CONTEXT+0].xyz);
			t.yz=dot(p,c[C_VIEW_CONTEXT+1].xyz);
			t.xyz=t.xyz*C_HALF+C_HALF;
*/		#endif
		return	t;
	}
	//Material
	half2 MaterialMat(in float2 t)
	{
		half3	p=half3(t.x,t.y,C_UNIT);
		half2	v;
		v.x=dot(p,c[C_MTL_CONTEXT+0].xyz);
		v.y=dot(p,c[C_MTL_CONTEXT+1].xyz);
		return	v;
	}

	// Texture Space
	float3x3	LightMat(in const float3 vTangent,in const float3 vBinormal,in const float3 vNormal)
	{
		float3x3	m=float3x3(	vTangent.x,vBinormal.x,vNormal.x,
								vTangent.y,vBinormal.y,vNormal.y,
								vTangent.z,vBinormal.z,vNormal.z);
		return	m;
	}
	float3x3	LightMat(in const float4 inTangent,in const float3 inNormal,uniform bool bCompTangent=true)
	{
		if(bCompTangent)		//inCompTangent a ramené entre -1/1 ?
		{
			float4		vTangent=inTangent*C_TWO-C_UNIT;
			//vTangent.w = handedness
			float3		vBinormal = cross(inNormal,float3(vTangent.x,vTangent.y,vTangent.z)) * vTangent.w;
			return	LightMat(vTangent,vBinormal,inNormal);
		}
		float3		vTangent=float3(inTangent.x,inTangent.y,inTangent.z);
		float3		vBinormal=cross(inNormal,vTangent) * inTangent.w;
		return	LightMat(vTangent,vBinormal,inNormal);
	}


	//Materials & Radiosity
	void	BaseMaterial(in float2 diffuse,in float2 radiosity,out S_TexCoord o)
	{
#ifdef	bVLight
		o.diffuse.xy	= MaterialMat(diffuse);
	#ifdef	bRadiosity
		#ifndef	bNormal
		o.radiosity.xy	= radiosity;
		#else
		o.radiosity.xy	= radiosity*C_HALF;
		#endif
	#endif
#else
		o.diffuse.xy	= MaterialMat(diffuse);
	#ifdef	bRadiosity
		#ifndef	bNormal
		o.diffuse.zw	= radiosity;
		#else
		o.diffuse.zw	= radiosity*C_HALF;
		#endif
	#else
		o.diffuse.zw	= C_NULL;			//ptet faire le define la, pff
	#endif
#endif
	}


#ifndef	bVLight
	half2	RadTexCoord(in float2 radiosity)
	{
		#ifndef	bNormal
		return	radiosity;
		#else
		return	radiosity*C_HALF;
		#endif
	}
#endif
#ifdef bRadiosity
	half3	RadMap(const S_TexCoord i)
	{
	#ifdef	bVLight
		return	tex2D(sRadiosityMap,i.radiosity.xy);
	#else
		return	tex2D(sRadiosityMap,i.diffuse.zw);
	#endif
	}
#endif

	// Get Luminance from RGB values
	float	Luminance(in const float4 c)
	{
		return 0.27f*c.r + 0.67f*c.g + 0.06f*c.b;
	}

	void	TextureScreen(in float4 pos,out float2 uv,bool bFlip)
	{
		uv = pos.xy/pos.w;
		uv = max(uv,-C_UNIT);
		uv = min(uv,C_UNIT);
		if(bFlip)
			uv.y *= -C_UNIT;
		uv = (uv*VIEWPORT_HALF_SIZE+VIEWPORT_RDR_CENTER)*INV_RDR_SIZE;
	}
	 
#ifdef _X360
	float4 gHDRSettings;

	float3 PackHDRSample( float3 hdrSample )
	{
		//hdrSample = min( hdrSample, 254.0f );

		return float3(	hdrSample.r / (gHDRSettings.w + hdrSample.r),
						hdrSample.g / (gHDRSettings.w + hdrSample.g),
						hdrSample.b / (gHDRSettings.w + hdrSample.b) );
	}

	float3 UnpackHDRSample( float4 packedHDR )
	{
		//packedHDR.rgb = min( packedHDR.rgb, 254.0f / 255.0f );
		return float3(	packedHDR.r / (1.000001f - packedHDR.r),
						packedHDR.g / (1.000001f - packedHDR.g),
						packedHDR.b / (1.000001f - packedHDR.b) ) * gHDRSettings.w;
	}
#endif

#endif
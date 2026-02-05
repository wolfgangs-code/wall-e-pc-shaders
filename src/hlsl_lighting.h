#ifndef	_HLSL_LIGHTING_H
#define	_HLSL_LIGHTING_H

	half4 Fog(in const float4 inPos)
	{
		float4	v;
		v.xyz=FOG_COLOR;
		v.w=FOG_ON*(C_UNIT-clamp(dot(inPos,FOG_PLANE),C_NULL,C_UNIT));
		return	v;
	}

#ifdef	bVLight
	void		VertexLighting(in const float4 position,in const float3 normal,inout VS_Output o)
	{
		#ifdef	bSpecular
			float3	eye =normalize(EYE_LOCAL-position);
			float3	ref =dot(normal,eye)*normal*C_TWO-eye;
			o.eyevec=pow(clamp(dot(ref,DLIGHT_DIR),C_NULL,C_UNIT),MTL_SPECULAR_POW)*MTL_SPECULAR*DLIGHT_COLOR;
		#endif

			o.light.w=MTL_EMISSIVE.w*GOURAUD_DFLT.w;
	#ifndef	bNoDLight
			half4	dl=(max(C_NULL,dot(DLIGHT_DIR,normal)))*DLIGHT_COLOR+DLIGHT_AMBIENT;
			o.light.xyz=(dl*MTL_DIFFUSE+MTL_EMISSIVE)*GOURAUD_DFLT;
	#else
			o.light.xyz=C_NULL;
		#ifdef	bSpecular
			o.eyevec*=DLIGHT_RADSPECULAR;
		#endif
	#endif

#ifdef	bOmni
			float3		omni0=position-OMNI_0_POS;
			float3		omni1=position-OMNI_1_POS;
			float3		omni2=position-OMNI_2_POS;
			//sqr dist
			float3		range;
			range.x=dot(omni0,omni0);
			range.y=dot(omni1,omni1);
			range.z=dot(omni2,omni2);
			//dir
			omni0.xyz*=-rsqrt(range.x);
			omni1.xyz*=-rsqrt(range.y);
			omni2.xyz*=-rsqrt(range.z);

			//range omni
			range.x=C_UNIT-((range.x-OMNI_0_STARTSQR)*OMNI_0_ATT);
			range.y=C_UNIT-((range.y-OMNI_1_STARTSQR)*OMNI_1_ATT);
			range.z=C_UNIT-((range.z-OMNI_2_STARTSQR)*OMNI_2_ATT);

			//normal
			float3	facing;
			facing.x=dot(normal,omni0);
			facing.y=dot(normal,omni1);
			facing.z=dot(normal,omni2);
			facing.xyz=max(C_NULL,facing.xyz);

			//cutoff
			float3	cutoff;
			cutoff.x=dot(omni0,OMNI_0_DIR)*OMNI_0_CUTOFF0+OMNI_0_CUTOFF1;
			cutoff.y=dot(omni1,OMNI_1_DIR)*OMNI_1_CUTOFF0+OMNI_1_CUTOFF1;
			cutoff.z=dot(omni2,OMNI_2_DIR)*OMNI_2_CUTOFF0+OMNI_2_CUTOFF1;
			cutoff.xyz=max(C_NULL,C_UNIT-cutoff.xyz);

			range*=facing;
			range*=cutoff;
			range=clamp(range,C_NULL,C_UNIT);
			
			float3	o0=OMNI_0_COLOR.xyz*range.x*OMNI_0_RADDIFFUSE;
			float3	o1=OMNI_1_COLOR.xyz*range.y*OMNI_1_RADDIFFUSE;
			float3	o2=OMNI_2_COLOR.xyz*range.z*OMNI_2_RADDIFFUSE;
			o.light.xyz+=(o0+o1+o2)*MTL_DIFFUSE;

	#ifdef	bSpecular
			range.x=dot(ref,omni0)*OMNI_0_RADSPECULAR;
			range.y=dot(ref,omni1)*OMNI_1_RADSPECULAR;
			range.z=dot(ref,omni2)*OMNI_2_RADSPECULAR;
			range=clamp(range,C_NULL,C_UNIT);
			o0*=pow(range.x,MTL_SPECULAR_POW);
			o1*=pow(range.y,MTL_SPECULAR_POW);
			o2*=pow(range.z,MTL_SPECULAR_POW);
			o.eyevec+=(o0+o1+o2)*MTL_SPECULAR*MTL_DIFFUSE;
	#endif
#endif
			o.light.xyz=min(C_UNIT,o.light.xyz);
			//Fog As Specular Color, added after fixed pipeline, so substract Fog on Diffuse Color
//			o.fog			=Fog(position);
//			o.light.xyz		-=o.fog.w*o.light.xyz;
//			o.fog.xyz		=o.fog.w*FOG_COLOR;
	}

//-----------------------------------------------------------------------------
#else
//-----------------------------------------------------------------------------
	float4 ProjSpotLight0(in const float4 inPos)
	{
		float4x4	pm=float4x4(
			c[C_OMNI_0_PROJMATTEX].x,c[C_OMNI_0_PROJMATTEX+1].x,c[C_OMNI_0_PROJMATTEX+2].x,c[C_OMNI_0_PROJMATTEX+3].x,
			c[C_OMNI_0_PROJMATTEX].y,c[C_OMNI_0_PROJMATTEX+1].y,c[C_OMNI_0_PROJMATTEX+2].y,c[C_OMNI_0_PROJMATTEX+3].y,
			c[C_OMNI_0_PROJMATTEX].z,c[C_OMNI_0_PROJMATTEX+1].z,c[C_OMNI_0_PROJMATTEX+2].z,c[C_OMNI_0_PROJMATTEX+3].z,
			c[C_OMNI_0_PROJMATTEX].w,c[C_OMNI_0_PROJMATTEX+1].w,c[C_OMNI_0_PROJMATTEX+2].w,c[C_OMNI_0_PROJMATTEX+3].w);
		return	mul(inPos,pm);
	}
	float4 ProjSpotLight1(in const float4 inPos)
	{
		float4x4	pm=float4x4(
			c[C_OMNI_1_PROJMATTEX].x,c[C_OMNI_1_PROJMATTEX+1].x,c[C_OMNI_1_PROJMATTEX+2].x,c[C_OMNI_1_PROJMATTEX+3].x,
			c[C_OMNI_1_PROJMATTEX].y,c[C_OMNI_1_PROJMATTEX+1].y,c[C_OMNI_1_PROJMATTEX+2].y,c[C_OMNI_1_PROJMATTEX+3].y,
			c[C_OMNI_1_PROJMATTEX].z,c[C_OMNI_1_PROJMATTEX+1].z,c[C_OMNI_1_PROJMATTEX+2].z,c[C_OMNI_1_PROJMATTEX+3].z,
			c[C_OMNI_1_PROJMATTEX].w,c[C_OMNI_1_PROJMATTEX+1].w,c[C_OMNI_1_PROJMATTEX+2].w,c[C_OMNI_1_PROJMATTEX+3].w);
		return	mul(inPos,pm);
	}
	float4 ProjSpotLight2(in const float4 inPos)
	{
		float4x4	pm=float4x4(
			c[C_OMNI_2_PROJMATTEX].x,c[C_OMNI_2_PROJMATTEX+1].x,c[C_OMNI_2_PROJMATTEX+2].x,c[C_OMNI_2_PROJMATTEX+3].x,
			c[C_OMNI_2_PROJMATTEX].y,c[C_OMNI_2_PROJMATTEX+1].y,c[C_OMNI_2_PROJMATTEX+2].y,c[C_OMNI_2_PROJMATTEX+3].y,
			c[C_OMNI_2_PROJMATTEX].z,c[C_OMNI_2_PROJMATTEX+1].z,c[C_OMNI_2_PROJMATTEX+2].z,c[C_OMNI_2_PROJMATTEX+3].z,
			c[C_OMNI_2_PROJMATTEX].w,c[C_OMNI_2_PROJMATTEX+1].w,c[C_OMNI_2_PROJMATTEX+2].w,c[C_OMNI_2_PROJMATTEX+3].w);
		return	mul(inPos,pm);
	}
	float4 ProjSpotLight3(in const float4 inPos)
	{
		float4x4	pm=float4x4(
			c[C_OMNI_3_PROJMATTEX].x,c[C_OMNI_3_PROJMATTEX+1].x,c[C_OMNI_3_PROJMATTEX+2].x,c[C_OMNI_3_PROJMATTEX+3].x,
			c[C_OMNI_3_PROJMATTEX].y,c[C_OMNI_3_PROJMATTEX+1].y,c[C_OMNI_3_PROJMATTEX+2].y,c[C_OMNI_3_PROJMATTEX+3].y,
			c[C_OMNI_3_PROJMATTEX].z,c[C_OMNI_3_PROJMATTEX+1].z,c[C_OMNI_3_PROJMATTEX+2].z,c[C_OMNI_3_PROJMATTEX+3].z,
			c[C_OMNI_3_PROJMATTEX].w,c[C_OMNI_3_PROJMATTEX+1].w,c[C_OMNI_3_PROJMATTEX+2].w,c[C_OMNI_3_PROJMATTEX+3].w);
		return	mul(inPos,pm);
	}
	float4 ProjSpotLight4(in const float4 inPos)
	{
		float4x4	pm=float4x4(
			c[C_OMNI_4_PROJMATTEX].x,c[C_OMNI_4_PROJMATTEX+1].x,c[C_OMNI_4_PROJMATTEX+2].x,c[C_OMNI_4_PROJMATTEX+3].x,
			c[C_OMNI_4_PROJMATTEX].y,c[C_OMNI_4_PROJMATTEX+1].y,c[C_OMNI_4_PROJMATTEX+2].y,c[C_OMNI_4_PROJMATTEX+3].y,
			c[C_OMNI_4_PROJMATTEX].z,c[C_OMNI_4_PROJMATTEX+1].z,c[C_OMNI_4_PROJMATTEX+2].z,c[C_OMNI_4_PROJMATTEX+3].z,
			c[C_OMNI_4_PROJMATTEX].w,c[C_OMNI_4_PROJMATTEX+1].w,c[C_OMNI_4_PROJMATTEX+2].w,c[C_OMNI_4_PROJMATTEX+3].w);
		return	mul(inPos,pm);
	}

	void OutTexSpace(out S_TexSpace o,in float3x3 mLight,in float3 pos,in float3 normal)
	{
#ifdef	bNormal
		float3x3 ImLight	= transpose(mLight);
		o.axe0.xyz	=ImLight[0];
		o.axe1.xyz	=ImLight[1];
		o.axe2.xyz	=ImLight[2];
		o.axe0.w	=pos.x;
		o.axe1.w	=pos.y;
		o.axe2.w	=pos.z;
#else
	#ifndef	bNormalLocal
		o.normal	=normal;
	#endif
		o.pos		=pos;
#endif
	}

#ifdef	bOmni
	void Out_Omnis(inout VS_Output_Omni o,in const float4 VertexWorld)
	{
		#ifdef	bOmni12
			o.omni0tcproj= ProjSpotLight1(VertexWorld);
			o.omni1tcproj= ProjSpotLight2(VertexWorld);
		#endif
		#ifdef	bOmni34
			o.omni0tcproj= ProjSpotLight3(VertexWorld);
			o.omni1tcproj= ProjSpotLight4(VertexWorld);
		#endif
	}
#endif
#endif

	#include "scattering.h"
	
	float3 lighting(	const float3 normal,	//Déja Normé
						const float3 sun,		//Déja Normé
						const float3 sky,		//Déja Normé
						const float3 eye,		//Déja Normé
						const float3 suncolor,const float3 skycolor,const float3 color,
						const float speclevel,
						const float shadow)
	{
		//Specular
		float3	skyn = sky;
		float3	sunn = sun;
		float3	ref = dot(normal,eye)*normal*2-eye;
//		float3	ref = reflect( -eyevec, normal );
		float	s = saturate(dot(ref,sunn));
		s = pow(s,15)*speclevel;	// On met 3x plus pour permettre de cramer et atteindre la luminance du soleil
		//Diffuse
		float	sunv = saturate(dot(normal,sunn));
		s*=shadow;
		sunv*=shadow;
		//Diffuse 2
		float	skyv=(dot(skyn,normal)*0.25+0.75);
		float3	diffuselight=skyv*skycolor+sunv*suncolor;
		return (diffuselight*color+s*suncolor);
	}

#endif

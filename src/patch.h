#ifndef	_PATCH_H
#define	_PATCH_H

	#include "hlsl_math.h"

	struct VS_Input_Patch {
		float4	incs		: POSITION;
		float4	maddv		: POSITION1;
		float4	maddu		: POSITION2;
		float2	maddnu		: POSITION3;
	};

	#define V_INC				i.incs.x
	#define V_INC2				i.incs.y
	#define V_INC3				i.incs.z
	#define	V_MUL3				i.incs.w

	#define	V_MUL6				i.maddv.x
	#define V_MADD_V0			i.maddv.y
	#define V_MADD_V1			i.maddv.z
	#define V_MADD_V2			i.maddv.w

	#define V_MADD_U0			i.maddu.x
	#define V_MADD_U1			i.maddu.y
	#define V_MADD_U2			i.maddu.z
	#define V_MADD_NINCU		i.maddu.w

	#define V_MADD_NINCV		i.maddnu.x
	#define	V_MADD_NINCU_INC2	i.maddnu.y

/*
#define		PACTH_CONTROL(oREG,iCTRLA,iCTRLB,iCTRLC,iCTRLD)\
	mul		R_TEMP,		iCTRLA	,	V_MUL6\
	mul		R_TEMP,		R_TEMP	,	V_INC3\
	mad		oREG,		R_TEMP	,	V_MADD_V2	,iCTRLD\
	\
	mul		R_TEMP2,	iCTRLB	,	V_INC2\
	mad		R_TEMP2,	C_TWO	,	R_TEMP2,	R_TEMP\
	mad		oREG,		R_TEMP2	,	V_MADD_V1	,oREG\
	\
	mul		R_TEMP,		iCTRLA	,	V_INC3\
	mad		R_TEMP,		iCTRLB	,	V_INC2		,R_TEMP\
	mad		R_TEMP,		iCTRLC	,	V_INC		,R_TEMP\
	mad		oREG,		R_TEMP	,	V_MADD_V0	,oREG\
*/	float3	control(in const VS_Input_Patch i,in const float3 CtrlA,in const float3 CtrlB,in const float3 CtrlC,in const float3 CtrlD)
	{
		float3	o1=(CtrlA*V_MUL6)*V_INC3;
		float3	o2=((CtrlB*V_INC2)*C_TWO)+o1;
		float3	o=o1*V_MADD_V2+CtrlD;
			o+=o2*V_MADD_V1;
		float3	o3=CtrlA*V_INC3;
				o3+=CtrlB*V_INC2;
				o3+=CtrlC*V_INC;
		return	o+o3*V_MADD_V0;
	}
/*#define		PATCH_INTERPOL(oREG,iVAL0,iVAL1_S0,iVAL2_A0_S1_S3,iVAL3_S0)\
	mul		R_TEMP,		iVAL3_S0,	V_INC\
	mad		oREG,		R_TEMP,		V_MADD_NINCU,	iVAL0\
	\
	mul		R_TEMP,		iVAL1_S0,			V_INC\
	mad		R_TEMP,		iVAL2_A0_S1_S3,		V_MADD_NINCU_INC2,	R_TEMP\
	\
	mad		oREG,		R_TEMP,		V_MADD_NINCV,	oREG\
*/
	float3	interpol_xyz(in const VS_Input_Patch i,in const float3 iVAL0,in const float3 iVAL1_S0,in const float3 iVAL2_A0_S1_S3,in const float3 iVAL3_S0)
	{
		float3	o1=iVAL3_S0*V_INC;
		float3	o=o1*V_MADD_NINCU+iVAL0;
				o1=iVAL1_S0*V_INC;
				o1+=iVAL2_A0_S1_S3*V_MADD_NINCU_INC2;
		return	o1*V_MADD_NINCV+o;
	}
	float4	interpol_xyzw(in const VS_Input_Patch i,in const float4 iVAL0,in const float4 iVAL1_S0,in const float4 iVAL2_A0_S1_S3,in const float4 iVAL3_S0)
	{
		float4	o1=iVAL3_S0*V_INC;
		float4	o=o1*V_MADD_NINCU+iVAL0;
				o1=iVAL1_S0*V_INC;
				o1+=iVAL2_A0_S1_S3*V_MADD_NINCU_INC2;
		return	o1*V_MADD_NINCV+o;
	}
/*
		mul		R_TEMP,R_START0,V_MUL3\
	mul		R_TEMP2	,	R_START1	,V_MUL3\
	\
	add		oNRM,	R_TEMP2	,	-R_TEMP\
	\
	add		oUV	,-R_TEMP2	,-oNRM\
	mad		oUV	,R_START2	,V_MUL3		,oUV\
	\
	add		oRGBA,		R_TEMP2,	-R_START0\
	mad		oRGBA,		R_START2,	-V_MUL3,	oRGBA\
	add		oRGBA,		R_START3,	oRGBA\
	\
	mul		R_TEMP,		oUV,		V_INC2\
	mul		R_TEMP2,	oRGBA,		V_INC3\
	\
	mul		R_START3,	R_TEMP2,	V_MUL6\
	add		R_START2,	R_TEMP,		R_TEMP\
	\
	add		R_TEMP	,	R_TEMP,		R_TEMP2\
	mad		R_START1,	oNRM,		V_INC,		R_TEMP\
	\
	mad		R_START0,	R_START1,	V_MADD_U0,	R_START0\
	mad		R_START0,	R_START2,	V_MADD_U1,	R_START0\
	mad		oPOS,		R_START3,	V_MADD_U2,	R_START0\
	mov		oPOS.w,	C_UNIT\
*/
	float4	position(in const VS_Input_Patch i,in float3 r0,in float3 r1,in float3 r2,in float3 r3)
	{
		float3	rtemp=r0*V_MUL3;
		float3	rtemp2=r1*V_MUL3;
		float3	nrm=rtemp2-rtemp;
		float3	uv=(r2*V_MUL3)-(rtemp2+nrm);
		float3	rgba=r3+(rtemp2-r0)-(r2*V_MUL3);

		uv*=V_INC2;

		rgba*=V_INC3;
		//rgba*=V_MUL6;

		r3=rgba*V_MUL6;
		r2=uv*C_TWO;

		uv+=rgba;
		r1=nrm*V_INC+uv;

		r0+=r1*V_MADD_U0;
		r0+=r2*V_MADD_U1;
		r0+=r3*V_MADD_U2;
		float4	o;
		o.xyz=r0;
		o.w=C_UNIT;
		return	o;
	}

#endif

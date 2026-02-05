
#ifdef	bShadowMap		
#ifdef _X360
	//#define bShadowMapNoise
#endif
#endif

	struct VS_Output_ShadowMap {
		float4	position	: POSITION;
		float4	uvzw		: TEXCOORD0;
	};

	float	DebugShadowExtent( in const float4 tcproj )
	{
		return (	(tcproj.x > 0.0f) && (tcproj.x < 1.0f) &&
					(tcproj.y > 0.0f) && (tcproj.y < 1.0f) ) ? 1.0f : 0.0f;
	}

	float	PixelShadowDebug(in const float4 tcproj)
	{
		return 1 - tex2Dproj( sShadowMap, tcproj ).x;
	}

	float	PixelShadow1(in const float4 tcproj)
	{
		return step( tex2Dproj( sShadowMap, tcproj ).x, tcproj.z/tcproj.w );  
	}
	float	PixelShadow4(in const float4 tcproj)
	{
		float invsize=gShadowMap.y;
		float sourcevals[4];
		float	itcprojw=1/tcproj.w;
		float4	projtcproj=tcproj*itcprojw;
		sourcevals[0] = step( tex2D( sShadowMap, projtcproj ), projtcproj.z );  
		sourcevals[1] = step( tex2D( sShadowMap, projtcproj + float2(invsize, 0) ), projtcproj.z );  
		sourcevals[2] = step( tex2D( sShadowMap, projtcproj + float2(0, invsize) ), projtcproj.z );  
		sourcevals[3] = step( tex2D( sShadowMap, projtcproj + float2(invsize, invsize) ), projtcproj.z );  
		// transform to texel space
		float2 texelpos = gShadowMap.x * projtcproj;
		// Determine the lerp amounts           
		float2 lerps = frac( texelpos );
		// lerp between the shadow values to calculate our light amount
		float LightAmount = lerp( lerp( sourcevals[0], sourcevals[1], lerps.x ),
								  lerp( sourcevals[2], sourcevals[3], lerps.x ),
								  lerps.y );
		return LightAmount;	                            
	}
	float	PixelShadow9(in const float4 tcproj)
	{
		float blur=1.f;
		float invsize=gShadowMap.y*blur;
		float	itcprojw=1/tcproj.w;
		float4	projtcproj=tcproj*itcprojw;
		float nshadowed=0;
		nshadowed=  step( tex2D( sShadowMap, projtcproj + float2(-invsize,-invsize)), projtcproj.z );  
		nshadowed+= step( tex2D( sShadowMap, projtcproj + float2(-invsize, 0)),       projtcproj.z );  
		nshadowed+= step( tex2D( sShadowMap, projtcproj + float2(-invsize, invsize)), projtcproj.z );  
		nshadowed+= step( tex2D( sShadowMap, projtcproj + float2( 0	, -invsize)),     projtcproj.z );  
		nshadowed+= step( tex2D( sShadowMap, projtcproj ),                            projtcproj.z );  
		nshadowed+= step( tex2D( sShadowMap, projtcproj + float2( 0	, invsize)),      projtcproj.z );  
		nshadowed+= step( tex2D( sShadowMap, projtcproj + float2( invsize,-invsize)), projtcproj.z );  
		nshadowed+= step( tex2D( sShadowMap, projtcproj + float2( invsize, 0)),       projtcproj.z );  
		nshadowed+= step( tex2D( sShadowMap, projtcproj + float2( invsize, invsize)), projtcproj.z );  
		return nshadowed/9.0;
	}

	float2	ComputeShadowNoiseUV( in const float4 vertexShadowNoiseUv )
	{
		#define NOISE_TEX_SIZE 32

		float2 uv = vertexShadowNoiseUv.xy / vertexShadowNoiseUv.w;
	
		uv = ( uv + 1.0f ) * 0.5f;
		uv *= float2( 1280.0f / NOISE_TEX_SIZE, 720.0f / NOISE_TEX_SIZE );
		uv += 0.5f / NOISE_TEX_SIZE;

		return uv;
	}
#ifdef bShadowMapNoise

	const float4 gShadowSetting;

	float	PixelShadowNoise(in const float4 _tcproj, in const float2 _noiseUV, in const float z )
	{

		#define SHADOW_MAP_RES 1024

		#define SOFT_SHADOW_NOISE_RES		32
		#define SOFT_SHADOW_NUM_SAMPLES_A	10
		#define SOFT_SHADOW_NUM_SAMPLES_R	4

		#define SOFT_SHADOW_NUM_SAMPLES		( SOFT_SHADOW_NUM_SAMPLES_R * SOFT_SHADOW_NUM_SAMPLES_A )
		#define SOFT_SHADOW_NUM_TEST_SAMPLE	SOFT_SHADOW_NUM_SAMPLES_A
		
		//if( z > gShadowSetting.x )
		//	return PixelShadow4( _tcproj );
		
		float3 noiseUV;
		noiseUV.xy = _noiseUV;
		noiseUV.z = 0.1f;	

		float light = step( tex2D( sShadowMap, _tcproj ), _tcproj.z );

		const float filterSize = gShadowSetting.y / SHADOW_MAP_RES;

		float4 tcproj = _tcproj;

		for( int i=0 ; i < (SOFT_SHADOW_NUM_TEST_SAMPLE/2) ; ++i )
		{
			float4 offsets = tex3D( sShadowNoiseMap, noiseUV ) * 2.0f - 1.0f;
			offsets *= filterSize;
			
			tcproj.xy = _tcproj.xy + offsets.rg;
			light += step( tex2D( sShadowMap, tcproj ), _tcproj.z );

			tcproj.xy = _tcproj.xy + offsets.ba;
			light += step( tex2D( sShadowMap, tcproj ), _tcproj.z );

	
			noiseUV.z += ( 2.0f / SOFT_SHADOW_NUM_SAMPLES );
		}
		
		light *= ( 1.0f / ( SOFT_SHADOW_NUM_TEST_SAMPLE + 1 ) );
		
		//return light;
		
		if( light * ( 1.0f - light ) == 0.0f )
		{
			//We're certainly completely lit or completely in shadow 
			return light;
		}
		else
		{
			//We're in penumbra, take more samples to refine the result
			
			light *= ( SOFT_SHADOW_NUM_TEST_SAMPLE + 1 );
			
			for( int i=0 ; i < ((SOFT_SHADOW_NUM_SAMPLES - SOFT_SHADOW_NUM_TEST_SAMPLE)/2) ; ++i )
			{
				float4 offsets = tex3D( sShadowNoiseMap, noiseUV ) * 2.0f - 1.0f;
				offsets *= filterSize;
				
				tcproj.xy = _tcproj.xy + offsets.rg;
				light += step( tex2D( sShadowMap, tcproj ), _tcproj.z );

				tcproj.xy = _tcproj.xy + offsets.ba;
				light += step( tex2D( sShadowMap, tcproj ), _tcproj.z );
						
				noiseUV.z += ( 2.0f / SOFT_SHADOW_NUM_SAMPLES );
			}	
			
			return light * ( 1.0f / SOFT_SHADOW_NUM_SAMPLES );
		}
	}
#endif

    float3 AddShadowBias( in float3 pos, in float3 normal )
	{
		return pos;
/*
		float displaceAlongNormal = C_UNIT - abs( dot( DLIGHT_DIR, normal ) );
		pos.xyz += normal * (displaceAlongNormal * SHADOWMAP_VERTEX_BIAS);
		pos.xyz += DLIGHT_DIR * SHADOWMAP_VERTEX_ADDBIAS;
		return pos;*/
	}
    float4 SubShadowBias( in const float4 pos )
	{
		float4	retpos=pos;
		retpos.xyz+=DLIGHT_DIR*SHADOWMAP_VERTEX_ADDBIAS;
		return retpos;
/*
		float displaceAlongNormal = C_UNIT - abs( dot( DLIGHT_DIR, normal ) );
		pos.xyz += normal * (displaceAlongNormal * SHADOWMAP_VERTEX_BIAS);
		pos.xyz += DLIGHT_DIR * SHADOWMAP_VERTEX_ADDBIAS;
		return pos;*/
	}
    float4 ReAddShadowBias( in const float4 pos )
	{
		float4	retpos=pos;
		retpos.xyz-=DLIGHT_DIR*SHADOWMAP_VERTEX_ADDBIAS;
		return retpos;
/*
		float displaceAlongNormal = C_UNIT - abs( dot( DLIGHT_DIR, normal ) );
		pos.xyz += normal * (displaceAlongNormal * SHADOWMAP_VERTEX_BIAS);
		pos.xyz += DLIGHT_DIR * SHADOWMAP_VERTEX_ADDBIAS;
		return pos;*/
	}

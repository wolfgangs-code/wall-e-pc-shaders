	
	//-------------------------------------------------
	// Atmospheric scattering

	// !! Shared
	// Attention, partagé entre la PS3, la XBox 360 et le PC.
	// CG est moins permissif, donc écrire les choses correctement (avec les swizzles et tout)

	// calcule un démultiplicateur intégral de densité athmosphérique entre deux points
	void statterintegral(in const float lookAtLen,in const float iPos1y,in const float iPos2y,out float integral)
	{
		float4	r11,r10;
		r11.xz = max(iPos1y,iPos2y);
		r11.yw = min(iPos1y,iPos2y);
		float4	or11 = r11;
		r11.xy *= C_TERM_MULTIPLIERS.w;
		r11.zw *= C_SCATTERCONSTANTS2.w;

		r10 = min(r11,C_UNIT);

		r11.xy = r11.xz-r11.yw;
		r11.zw = r10.xz-r10.yw;
		r11 = max(r11,C_TERM_MULTIPLIERS.w);
		r11.x = 1/r11.x;
		r11.y = 1/r11.y;
		
		r11.xy *= r11.zw;
		
		r10.yw += r10.xz;
		r10 = C_UNIT-r10*C_HALF;
		
		r10.y *= r11.x*C_SCATTERCONSTANTS2.y;
		r10.w *= r11.y*C_SCATTERCONSTANTS2.z;
		r10.y += r10.w;

		integral = r10.y*lookAtLen;
		
		float4	or10 = max(or11,C_SCATTERCONSTANTS1.y);
				or10 = min(or10,C_TERM_MULTIPLIERS.z);
		float	totheight = or11.x-or11.y;
		float	cloudheight = or10.x-or10.y;
 
		float	itotheight = 1/(max(totheight,C_INV16384));
		float	prop = lookAtLen*cloudheight*itotheight;
		integral += prop*C_HG.w;
	}

	void scatteringbase(in const float4 iPos, in const float muldensity, in const float3 sunlightdir,out float3 extinction,out float3 inscatter)
	{
		float3	lookAt = EYE_WORLD.xyz-iPos.xyz;
		float	lookAtLen = length(lookAt);
		lookAt /= lookAtLen;
		float CosTheta = min(C_NULL,dot(lookAt.xyz,sunlightdir.xyz));
			  CosTheta = C_QUARTER+C_SCATTERCONSTANTS1.x*(CosTheta-C_QUARTER);

		float	pntdensity;
		float	pPosy=iPos.y;
				pPosy = max(pPosy,C_NULL);
		statterintegral(lookAtLen,pPosy,EYE_WORLD.y,pntdensity);
		pntdensity *= muldensity;
		
		// s = length of lookAt vector

		// Theta - Scattering angles
		// Theta = Angle between LookAt and LightDirection
		
		/// Phase1(theta) = 1+cos^2(theta)
		float tweenf=CosTheta*CosTheta;
		float Phase1Theta = C_UNIT+ tweenf;

		// E - Total extinction
		// Extinction term E = -(beta_1+beta_2) * s * log_2 e
		extinction = -C_BETA_1_PLUS_2.xyz*C_SCATTERCONSTANTS2.x;
	
		extinction *=pntdensity;
		// E1 = e^(-(beta_1 + beta_2) * s)
		extinction.x = pow(C_TWO,extinction.x);
		extinction.y = pow(C_TWO,extinction.y);
		extinction.z = pow(C_TWO,extinction.z);

		// Phase2(theta) = (1-g^2)/(1+g-2g*cos(theta))^(3/2)
		// theta is 180 - actual theta (this corrects for sign)
		// ScatteringHG = [1-g^2, 1+g, 2g]
		float Phase2ThetaSun = rsqrt(C_HG.y+C_HG.z*CosTheta);
		Phase2ThetaSun = C_HG.x * (Phase2ThetaSun * Phase2ThetaSun * Phase2ThetaSun);
		// Inscattering = (Beta'_1 * Phase_1(theta) + Beta'_2 * Phase_2(theta)) * 
		//        [1-exp(-Beta_1*s).exp(-Beta_2*s)] / (Beta_1 + Beta_2)
		inscatter  = (C_BETA_DASH_1.xyz*Phase1Theta + C_BETA_DASH_2.xyz*Phase2ThetaSun);
		inscatter *= C_ONE_OVER_BETA_1_PLUS_2.xyz;
		inscatter *= C_UNIT-extinction;

		// Apply multipliers
		float	itweenf=C_UNIT-tweenf;
		float3	suncolor=DLIGHT_COLOR.xyz*tweenf+DLIGHT_AMBIENT.xyz*itweenf;
		inscatter *= (suncolor*C_TERM_MULTIPLIERS.y);///(1+sundensity*2);

		extinction*=extinction;
	}

	void scattering(in const float4 iPos, in const float muldensity, in const float3 sunlightdir,out float3 extinction,out float3 inscatter,out half3 sunlight,out half3 skylight)
	{
		scatteringbase(iPos,muldensity,sunlightdir,extinction,inscatter);
		sunlight = DLIGHT_COLOR.xyz*C_SCATTERCONSTANTS1.z;
		skylight = DSKY_COLOR.xyz;
	}

	//
	// Version simplifiée du scattering.
	// L'intégrale est calculée en externe et est stockée dans le vertex
	void fast_scattering(in const float atmdensity,in const float4 iPos, in const float muldensity, in const float3 sunlightdir,out float3 extinction,out float3 inscatter,out half3 sunlight,out half3 skylight)
	{
		float3	lookAt = normalize(EYE_WORLD.xyz-iPos.xyz);
		float CosTheta = min(C_NULL,dot(lookAt.xyz,sunlightdir.xyz));
			  CosTheta = C_QUARTER+C_SCATTERCONSTANTS1.x*(CosTheta-C_QUARTER);

		float	pntdensity = atmdensity*muldensity;

		// s = length of lookAt vector

		// Theta - Scattering angles
		// Theta = Angle between LookAt and LightDirection
		
		/// Phase1(theta) = 1+cos^2(theta)
		float tweenf = CosTheta*CosTheta;
		float Phase1Theta = C_UNIT+tweenf;

		// E - Total extinction
		// Extinction term E = -(beta_1+beta_2) * s * log_2 e
		extinction = -C_BETA_1_PLUS_2.xyz*C_SCATTERCONSTANTS2.x;
		extinction *= pntdensity;

		// E1 = e^(-(beta_1 + beta_2) * s)
		extinction.x = pow(C_TWO,extinction.x);
		extinction.y = pow(C_TWO,extinction.y);
		extinction.z = pow(C_TWO,extinction.z);

		// Phase2(theta) = (1-g^2)/(1+g-2g*cos(theta))^(3/2)
		// theta is 180 - actual theta (this corrects for sign)
		// ScatteringHG = [1-g^2, 1+g, 2g]
		float Phase2ThetaSun = rsqrt(C_HG.y+C_HG.z*CosTheta);
		Phase2ThetaSun = C_HG.x * (Phase2ThetaSun * Phase2ThetaSun * Phase2ThetaSun);
		// Inscattering = (Beta'_1 * Phase_1(theta) + Beta'_2 * Phase_2(theta)) * 
		//        [1-exp(-Beta_1*s).exp(-Beta_2*s)] / (Beta_1 + Beta_2)
		inscatter  = (C_BETA_DASH_1.xyz*Phase1Theta + C_BETA_DASH_2.xyz*Phase2ThetaSun);
		inscatter *= C_ONE_OVER_BETA_1_PLUS_2.xyz;
		inscatter *= C_UNIT-extinction;

		// Apply multipliers
		float	itweenf = C_UNIT-tweenf;
		float3	suncolor = DLIGHT_COLOR.xyz*tweenf+DLIGHT_AMBIENT.xyz*itweenf;
		inscatter *= (suncolor*C_TERM_MULTIPLIERS.y);///(1+sundensity*2);

//extinction = 1;
//inscatter = 0;
		sunlight = DLIGHT_COLOR.xyz*C_SCATTERCONSTANTS1.z;
		skylight = DSKY_COLOR.xyz;

		extinction*=extinction;
	}

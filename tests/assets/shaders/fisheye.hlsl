/*
	Simple fisheye 

	Based on 
	https://en.wikipedia.org/wiki/Fisheye_lens and 
	http://gamedev.stackexchange.com/a/20642

	Restrictions:
	theta <= PI/2

	@author Caiwan/IR
*/

cbuffer Fisheye{
	float zoom;
	float alpha;
	float theta;
}

Texture2D effectInput;
SamplerState SampleType;

struct PixelInputType
{
	float4 position : SV_POSITION; 
	float2 tex : TEXCOORD;
};


float4 fisheyeProc(PixelInputType input) : SV_TARGET
{
	float2 uv = (input.tex - 0.5) / (2.0 + zoom);
	float z = (1.0 + alpha) * sqrt(1.0 - uv.x * uv.x - uv.y * uv.y);
	
	/* 
	 * To use different lens distortions uncomment the one following formulas:
	 */
	
	// Gnomonical
	float a = 1.0 / (z * tan(theta));

	// Stereographic
	// float a = 1.0 / (2 * z * tan(theta * 0.5));

	//Equidistant 
	// float a = 1.0 / (z * theta);

	// Equisolid angle
	// float a = 1.0 / (2 * z * sin(theta * 0.5));

	// Orthographic
	// float a = 1.0 / sin(theta * 0.5);
	
	float2 uv2 =  frac(uv * a);
	return effectInput.Sample(SampleType, uv2);
}
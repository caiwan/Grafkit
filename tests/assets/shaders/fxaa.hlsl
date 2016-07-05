/**
* @author alteredq / http://alteredqualia.com/
* @author davidedc / http://www.sketchpatch.net/
*
* NVIDIA FXAA by Timothy Lottes
* http://timothylottes.blogspot.com/2011/06/fxaa3-source-released.html
* - WebGL port by @supereggbert
* http://www.glge.org/demos/fxaa/
*/


#ifndef FXAA_REDUCE_MIN
	#define FXAA_REDUCE_MIN   (1.0/ 128.0)
#endif
#ifndef FXAA_REDUCE_MUL
	#define FXAA_REDUCE_MUL   (1.0 / 8.0)
#endif
#ifndef FXAA_SPAN_MAX
	#define FXAA_SPAN_MAX     8.0
#endif

// ==========================================================

cbuffer FXAA
{
	float2 resolution;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

Texture2D effectInput;
SamplerState SampleType;

float4 FXAA(PixelInputType input) : SV_TARGET {

	float4 color = float4(0,0,0,1);
	float2 inverseVP = float2(1.0 / resolution.x, 1.0 / resolution.y);

	float3 rgbNW = effectInput.Sample(SampleType, input.tex + float2(-1.0, -1.0) * inverseVP).xyz;
	float3 rgbNE = effectInput.Sample(SampleType, input.tex + float2(1.0, -1.0) * inverseVP).xyz;
	float3 rgbSW = effectInput.Sample(SampleType, input.tex + float2(-1.0, 1.0) * inverseVP).xyz;
	float3 rgbSE = effectInput.Sample(SampleType, input.tex + float2(1.0, 1.0) * inverseVP).xyz;
	
	float4 texColor = effectInput.Sample(SampleType, input.tex);
	
	float3 rgbM = texColor.xyz;
	float3 luma = float3(0.299, 0.587, 0.114);
	
	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaM = dot(rgbM, luma);
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

	float2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

	float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) *
		(0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);

	float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
	dir = min(float2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
		max(float2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
			dir * rcpDirMin)) * inverseVP;

	float3 rgbA = 0.5 * (
		effectInput.Sample(SampleType, input.tex + dir * (1.0 / 3.0 - 0.5)).xyz +
		effectInput.Sample(SampleType, input.tex + dir * (2.0 / 3.0 - 0.5)).xyz);
	float3 rgbB = rgbA * 0.5 + 0.25 * (
		effectInput.Sample(SampleType, input.tex + dir * -0.5).xyz +
		effectInput.Sample(SampleType, input.tex + dir * 0.5).xyz);

	float lumaB = dot(rgbB, luma);
	if ((lumaB < lumaMin) || (lumaB > lumaMax))
		color = float4(rgbA, texColor.a);
	else
		color = float4(rgbB, texColor.a);
	
	// return float4(dir.x, dir.y, 0, 1.);
	return color;
}

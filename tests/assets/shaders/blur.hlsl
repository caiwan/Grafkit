#include<types.hlsl>

/* Taken from Musk/BrCr
*/

SamplerState sm:register(s1); // linear sampler
Texture2D input:register(t0); // output of blur x

cbuffer blurParams : register(b1)
{
	float smooth; // 0.0 -> box filter, > 0.0 for gaussian
	float size; // length of the blur (global)
    
	float samplesx; // number of samples to take
	float directionx; // direction of blur
	float powerx; // length of the blur
	
    float samplesy; // number of samples to take
	float directiony; // direction of blur
	float powery; // length of the blur
	
    float blur_intensity; // how much of blur to mix
	float scene_intensity; // how much of scene to mix
}

float weight(float x) {
	return pow(2.71828, -(x*x)*(smooth*smooth)*64.0);
}

float4 blury(float4 PositionSS : SV_Position, float2 t : TEXCOORD) : SV_TARGET0
{
	float samplesCount = samplesy * 255 + 1;
	float increment = 1.0 / samplesCount;
	float4 col = input.Sample(sm,t);

	float amount = size*size*powery;
	float pi = 355.0 / 113.0;
	float2 dir = float2(cos(directiony*pi),sin(directiony*pi));

	float wacc = weight(0);
	float4 acc = input.Sample(sm,t)*wacc;

	for (float x = increment; x<1.0; x += increment) {
		float w = weight(x);
		acc += input.Sample(sm,t + x*amount*dir)*w;
		wacc += w;
		w = weight(-x);
		acc += input.Sample(sm,t - x*amount*dir)*w;
		wacc += w;
	}

	return acc / wacc;
}

float4 blurx(float4 PositionSS : SV_Position, float2 t:TEXCOORD) : SV_TARGET0
{
	float samplesCount = samplesx * 255 + 1;
	float increment = 1.0 / samplesCount;
	float4 col = input.Sample(sm,t);

	float amount = size*size*powerx;
	float pi = 355.0 / 113.0;
	float2 dir = float2(cos(directionx*pi),sin(directionx*pi));

	float wacc = weight(0);
	float4 acc = input.Sample(sm,t)*wacc;

	for (float x = increment; x<1.0; x += increment) {
		float w = weight(x);
		acc += input.Sample(sm,t + x*amount*dir)*w;
		wacc += w;
		w = weight(-x);
		acc += input.Sample(sm,t - x*amount*dir)*w;
		wacc += w;
	}

	return acc / wacc;
}
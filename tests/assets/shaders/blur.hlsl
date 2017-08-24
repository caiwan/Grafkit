#include<types.hlsl>

/* Taken from Musk/BrCr
*/

SamplerState sm:register(s1); // linear sampler
Texture2D effectInput:register(t0); // output of blur x

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

float4 blury(FXPixelInputType input) : SV_TARGET0
{
	float4 PositionSS = input.position;
	float2 t = input.tex;

	float samplesCount = samplesy * 255 + 1;
	float increment = 1.0 / samplesCount;
	float4 col = effectInput.Sample(sm,t);

	float amount = size*size*powery;
	float pi = PI;
	float2 dir = float2(cos(directiony*pi),sin(directiony*pi));

	float wacc = weight(0);
	float4 acc = effectInput.Sample(sm,t)*wacc;

	for (float x = increment; x < 1.0; x += increment) {
		float w = weight(x);
		acc += effectInput.Sample(sm,t + x*amount*dir)*w;
		wacc += w;
		w = weight(-x);
		acc += effectInput.Sample(sm,t - x*amount*dir)*w;
		wacc += w;
	}

	return acc / wacc;
}

float4 blurx(FXPixelInputType input) : SV_TARGET0
{
	float4 PositionSS = input.position;
	float2 t = input.tex;

	float samplesCount = samplesx * 255 + 1;
	float increment = 1.0 / samplesCount;
	float4 col = effectInput.Sample(sm,t);

	float amount = size*size*powerx;
	float pi = PI;
	float2 dir = float2(cos(directionx*pi), sin(directionx*pi));

	float wacc = weight(0);
	float4 acc = effectInput.Sample(sm,t)*wacc;

	for (float x = increment; x < 1.0; x += increment) {
		float w = weight(x);
		acc += effectInput.Sample(sm,t + x*amount*dir)*w;
		wacc += w;
		w = weight(-x);
		acc += effectInput.Sample(sm,t - x*amount*dir)*w;
		wacc += w;
	}

	return acc / wacc;
}




// qnd 3x3 convolutional gauss blur
float4 blur3x3(FXPixelInputType input) : SV_TARGET
{
#if 0
	// gaussian
	float kernel[9] = {
		1., 2., 1.,
		2., 4., 2.,
		1., 2., 1.
	};
#else
	// moving average
float kernel[9] = {
	1., 1., 1.,
	1., 1., 1.,
	1., 1., 1.
};
#endif

	float4 res = float4(0,0,0,0);

	int w = 0, h = 0;
	effectInput.GetDimensions(w, h);

	float dx = 1.0 / float(w);
	float dy = 1.0 / float(h);

	int k = 0;
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			res += kernel[k++] * effectInput.Sample(sm, input.tex + float2(dx * float(i), dy * float(j)));
		}
	}

	return saturate(res / k);
}

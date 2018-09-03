#include <types.hlsl>

Texture2D effectInput;
SamplerState SampleType;

#define sampleCount 20

cbuffer CAParams
{
    float distort1; // lens coefficient k1, default: .5
    float distort2; // lens coefficient k2, default: .5
    float distort3; // lens coefficient k3, default: .5
    float distortg; // lens global power, default: .5
    float zoom; // zoom amount, default: .5
    float chrom; // chromatic abberation, default: .5
}

float2 distort(float2 uv, float k1, float k2, float k3, float z) {
	//convert
	uv = uv - .5;
	uv *= 2.0;
	//do stuff
	uv.y *= .9 / 1.6;
	uv *= z;
	float r = length(uv);
	float r2 = r*r;
	float r4 = r2*r2;
	float r8 = r4*r4;
	uv = uv*(1 + k1*r2 + k2*r4 + k3*r8);
	//convert back
	uv.y *= 1.6 / .9;
	uv /= 2.0;
	return uv + .5;
}

float4 mainPixel(FXPixelInputType input) : SV_TARGET0 {
	float2 t = input.tex;

	float k1 = distort1*2.0 - 1.0;
	float k2 = distort2*2.0 - 1.0;
	float k3 = distort3*2.0 - 1.0;
	float g = distortg*2.0;
	k1 *= g;
	k2 *= g;
	k3 *= g;
	float rf = chrom*2.0;
	float z = zoom*2.0;
	float bf = 1.0 / rf;

	float4 col = 0;
	float4 wa = 0;

	float increment = (1.0 / sampleCount);

	for (float f = -1.0; f<1.0; f += increment) {
		float x = lerp(rf,bf,f*.5 + .5);
		float4 w = float4(max(0,-f),1.0 - abs(f),max(0,f),1.0);
		wa += w;
		col += effectInput.Sample(SampleType,distort(t,k1*x,k2*x,k3*x,z))*w;
	}

	col /= wa;
	return col;
}

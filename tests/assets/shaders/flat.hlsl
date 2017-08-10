#include <types.hlsl>

SamplerState SampleType {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct PixelOutType {
	float4 diff : SV_TARGET0;
	float4 normal : SV_TARGET1;
	float4 view : SV_TARGET2;
};

// PixelShader
//------------------------------------------------------------------------------------
PixelOutType mainPixel(PixelInputType input)
{
	PixelOutType output;
	output.normal = input.normal;
	output.view = input.view;
	output.diff = input.color0;
	return output;
}

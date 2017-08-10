#include <types.hlsl>

SamplerState SampleType {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

Texture2D t_diffuse;
Texture2D t_alpha;
Texture2D t_normal;
Texture2D t_specular;
Texture2D t_shininess;

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
	output.diff = .5 * input.normal - .5;
	return output;
}

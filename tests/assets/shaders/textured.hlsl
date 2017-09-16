#include <types.hlsl>
// GLOBALS //
cbuffer MatrixBuffer
{
    WorldMatrices wolrd;
};

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


// PixelShader
//------------------------------------------------------------------------------------
PixelOutType mainPixel(PixelInputType input)
{
	PixelOutType output;
	output.normal = input.normal;
	output.view = input.view;
	output.diff = t_diffuse.Sample(SampleType, input.tex);
	return output;
}

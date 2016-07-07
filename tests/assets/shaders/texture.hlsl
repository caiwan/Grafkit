#include "commons/projection.hlsl"

// GLOBALS //



Texture2D diffuse;
SamplerState SampleType;


// PixelShader
//------------------------------------------------------------------------------------
float4 mainPixel(PixelInputType input) : SV_TARGET
{
	float4 textureColor = float4(0,0,0,1);
	textureColor = diffuse.Sample(SampleType, input.tex);
	//textureColor.x = 0;
	//textureColor.w = 1;
	return textureColor;
}

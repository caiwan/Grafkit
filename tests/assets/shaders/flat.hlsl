
// GLOBALS //
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
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

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 binormal : BINORMAL;

	float4 color0 : COLOR0;
	//float4 color1 : COLOR1;

	float4 tex : TEXCOORD0;
	float4 view : VIEW;
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

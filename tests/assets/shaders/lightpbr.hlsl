#include <types.hlsl>

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct Light
{
	int type;

	float4 position;
	float4 direction;

	float4 ambient;
	float4 diffuse;
	float4 specular;

	float4 param1, param2;
};

cbuffer light
{
	struct Light light;
}

cbuffer material_params
{
	float4 mat_param0, mat_param1, mat_param2;
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


TextureCube envmap;
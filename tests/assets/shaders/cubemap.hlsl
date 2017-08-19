#include <types.hlsl>

cbuffer MatrixBuffer {
	WorldMatrices matrices;
}

cbuffer ResolutionBuffer {
	Resolution res;
};

SamplerState SampleType
{
	Filter = ANISOTROPIC;
	AddressU = WRAP;
	AddressV = WRAP;
};

TextureCube skybox;

float4 mainPixel(FXPixelInputType input) : SV_TARGET
{
	matrix view = matrices.viewMatrix;
	view._m03 = 0.;
	view._m13 = 0.;
	view._m23 = 0.;
	view._m33 = 0.;

	float u = input.tex.x;
	float v = input.tex.y * res.aspect;
	float w = 1. / tan(res.fov *.5);

	float3 uvw = normalize(mul(- float4(u, v, w, 0), view).xyz);

	return skybox.SampleLevel(SampleType, uvw, 10);
}
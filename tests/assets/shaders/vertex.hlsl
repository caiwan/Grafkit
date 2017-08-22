#include <types.hlsl>

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer material_colors
{
	float4 mat_diffuse, mat_specular;
};


PixelInputType mainVertex(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;

	output.view = input.position;
	output.view = mul(output.view, worldMatrix);
	output.view = mul(output.view, viewMatrix);

	output.position = output.view;
	output.position = mul(output.position, projectionMatrix);

	output.view.xyz = output.view.xyz / output.view.w;

	input.normal.w = 0.0f;
	output.normal = input.normal;
	output.normal = mul(output.normal, worldMatrix);
	output.normal = mul(output.normal, viewMatrix);
	output.normal = mul(output.normal, projectionMatrix);
	output.normal = normalize(output.normal);

	input.tangent.w = 0.0f;
	output.tangent = input.tangent;
	output.tangent = mul(output.tangent, worldMatrix);
	output.tangent = mul(output.tangent, viewMatrix);
	output.tangent = mul(output.tangent, projectionMatrix);
	output.tangent = normalize(output.tangent);

	input.binormal.w = 0.0f;
	output.binormal = input.binormal;
	output.binormal = mul(output.binormal, worldMatrix);
	output.binormal = mul(output.binormal, viewMatrix);
	output.binormal = mul(output.binormal, projectionMatrix);
	output.binormal = normalize(output.binormal);

	output.tex.zw = float2(0.0f, 1.0f);
	output.tex.xy = input.tex.xy;

	output.color0 = mat_diffuse;
	output.color1 = mat_specular;
	
	return output;
}


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

#define cubeCount 2048

cbuffer cubes
{
    float4 cubePos[256];
}

PixelInputType mainVertex(
    VertexInputType input)
{
    PixelInputType output;

    input.position.w = 1.0f;

    float4 pos = input.position;

    float t = 256 * (float) input.group0 / cubeCount;
    float t0 = floor(t);
    float tt = frac(t);

    float4 p0 = cubePos[(int) t0];
    float4 p1 = cubePos[(int) t0 + 1];

    float4 p = pos + lerp(p0, p1, tt);
    pos = p;

    output.view = pos;
    output.view = mul(output.view, worldMatrix);
    output.view = mul(output.view, viewMatrix);

    output.position = output.view;
    output.position = mul(output.position, projectionMatrix);

    output.view.xyz = output.view.xyz / output.view.w;

    input.normal.w = 0.0f;
    output.normal = input.normal; // normals are flipped for some reason
    output.normal = mul(output.normal, worldMatrix);
    output.normal = mul(output.normal, viewMatrix);

    input.tangent.w = 0.0f;
    output.tangent = input.tangent;
    output.tangent = mul(output.tangent, worldMatrix);
    output.tangent = mul(output.tangent, viewMatrix);

    input.binormal.w = 0.0f;
    output.binormal.xyz = cross(output.normal, output.tangent);

    output.tex.zw = float2(0.0f, 1.0f);
    output.tex.xy = input.tex.xy;

    output.color0 = mat_diffuse;
    output.color1 = mat_specular;
	
    return output;
}


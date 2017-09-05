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
    output.normal = - input.normal; // normals are flipped for some reason
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


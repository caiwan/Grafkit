#include <types.hlsl>

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};


Texture2D tex_age;
Texture2D tex_velocity;
Texture2D tex_speed;
Texture2D tex_position;

cbuffer material_colors
{
    float4 mat_diffuse, mat_specular;
};

PixelInputType mainVertex(
    VertexInputType input)
{
    PixelInputType output;

    input.position.w = 1.0f;

    float4 pos = input.position;
	
    int w = 0; 
    int h = 0;
    tex_position.GetDimensions(w, h);

    int x = input.group0 % w;
    int y = input.group0 / w;

    float2 uv = float2(float(x) / float(w), float(y) / float(h));
	
    float4 pp = tex_position.SampleLevel(SampleType, uv, 0);
    pp = pp * .1;
    pos = pos + pp;
    pos.w = 1;

	/// --- 

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

    output.color0 = float4(uv.x, uv.y, 0, 1);
    output.color1 = mat_specular;
	
    return output;
}


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

// 64 x 64 array of cubes 
#define cubeCount 4096

cbuffer cube_params
{
    int cubew;
    int cubeh;
    float4 cubeHeight[64];
}

PixelInputType mainVertex(
    VertexInputType input)
{
    PixelInputType output;

    input.position.w = 1.0f;

    float4 pos = input.position;

    //float t = 256 * (float) input.group0 / cubeCount;
    int x = input.group0 / 64;
    int y = input.group0 % cubeh;
	
    float xt = (float) x / 64.;
    float yt = (float) y / 64.;
	
    float h = cubeHeight[x].x * cubeHeight[y].y;
    h *= 100;

    float4 pp = float4(0, 0, 0, 1);
    pp.x = cubew * 3 / 2 - x * 3,
	pp.y = h;
    pp.z = cubeh * 3 / 2 - y * 3;

    pos = pos + pp;

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

    output.color0 = float4((float)x / (float)64, (float)y / (float)64, 0, 1);
    output.color1 = mat_specular;
	
    return output;
}


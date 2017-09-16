#define PI 3.14159265359

struct WorldMatrices
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct Resolution
{
    float2 resolution;
    float aspect;
    float fov;
};

struct Light
{
    float4 position;
    float4 color;
    float4 param1, param2;
};

struct MaterialVertex
{
};

struct Material
{
};

struct VertexInputType
{

    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 binormal : BINORMAL;
    float2 tex : TEXCOORD;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 binormal : BINORMAL;
	
    float4 color0 : COLOR0;
    float4 color1 : COLOR1;
    float4 color3 : COLOR2;
	
    float4 tex : TEXCOORD0;

    float4 view : VIEW0;
};

struct PixelOutType
{
    float4 diff : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 view : SV_TARGET2;
    float4 emission : SV_TARGET3;
};

struct FXPixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

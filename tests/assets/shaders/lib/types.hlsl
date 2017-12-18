#define PI 3.14159265359

struct WorldMatrices
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct BoneMatrices
{
    matrix boneMatrix[64];
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

struct VertexInputType
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 binormal : BINORMAL;
    float2 tex : TEXCOORD;
    int group0 : BLENDINDICES0;
    int group1 : BLENDINDICES1;
    int group2 : BLENDINDICES2;
    int group3 : BLENDINDICES3;
    float weight0 : BLENDWEIGHT0;
    float weight1 : BLENDWEIGHT1;
    float weight2 : BLENDWEIGHT2;
    float weight4 : BLENDWEIGHT3;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 binormal : BINORMAL;
	
    float4 color0 : COLOR0;
    float4 color1 : COLOR1;
	
    float4 tex : TEXCOORD0;

    float4 view : VIEW;
};

struct PixelOutType
{
    float4 diff : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 view : SV_TARGET2;
    float4 emission : sv_TARGET3;
};

struct FXPixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};


struct EffectParamType
{
    float4 screen;
    float4 viewport;
};

struct SceneParamType
{
    float globalTime;
    float lastTime;
    float deltaTime;
    float _padding;
    int frameCount;
};

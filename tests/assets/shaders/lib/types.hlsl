#define PI 3.14159265359

struct VertexInputType
{
	float4 position : POSITION;
	float4 normal :	NORMAL;
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
	//float4 color1 : COLOR1;
	
	float4 tex : TEXCOORD0;
	float4 view : VIEW;
};

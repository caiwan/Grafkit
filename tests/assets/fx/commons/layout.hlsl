/*
Input layout definiciok
*/

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD;
};

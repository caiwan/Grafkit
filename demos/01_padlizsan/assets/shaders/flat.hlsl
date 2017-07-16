
// GLOBALS //
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};



// TYPEDEFS //

struct VertexInputType
{
	float4 position : POSITION;
	float4 normal : NORMAL;
	float2 tex : TEXCOORD;
};

struct PixelInputType
{
	float4 position : SV_POSITION; 
	float4 view : VIEW;
	float4 normal : NORMAL;
	float2 tex : TEXCOORD;
};


struct PixelOutType {
	float4 diff : SV_TARGET0;
	float4 normal : SV_TARGET1;
	float4 view : SV_TARGET2;
};

// VertexShader
//------------------------------------------------------------------------------------
PixelInputType mainVertex(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;

	output.position = input.position;
	
	output.position = mul(output.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.view = output.position;
	output.position = mul(output.position, projectionMatrix);
		
	output.normal = input.normal;
	output.normal = mul(output.normal, worldMatrix); 
	output.normal = mul(output.normal, viewMatrix);
	output.normal = normalize(output.normal);

	output.tex = input.tex;

	return output;
}

// PixelShader
//------------------------------------------------------------------------------------
PixelOutType mainPixel(PixelInputType input)
{
	PixelOutType output;
	output.diff = float4(0.5,0.5,0.5,1);
	output.normal = input.normal;
	output.view = input.view;
	return output;
}

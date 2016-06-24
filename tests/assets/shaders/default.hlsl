
// GLOBALS //
cbuffer ViewMatrices
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

Texture2D texture_diffuse;
SamplerState SampleType;

// TYPEDEFS //

// https://msdn.microsoft.com/en-us/library/windows/desktop/bb509647(v=vs.85).aspx

struct PixelInputType
{
	float4 position : SV_POSITION; 
	//float4 normal : SV_NORMAL;
	//float4 tangent : SV_NORMAL;
	//float4 bitangent : SV_BITANGENT;
	float2 tex : TEXCOORD;
};

struct VertexInputType
{
	float4 position : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 tex : TEXCOORD;
};


// VertexShader
//------------------------------------------------------------------------------------
PixelInputType mainVertex(VertexInputType input)
{
	PixelInputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = input.position;
	
	 output.position = mul(output.position, worldMatrix);
	 output.position = mul(output.position, viewMatrix);
	 output.position = mul(output.position, projectionMatrix);
		
	output.tex = input.tex;

	return output;
}

// PixelShader
//------------------------------------------------------------------------------------
float4 mainPixel(PixelInputType input) : SV_TARGET
{
	float4 textureColor = float4(0,0,0,1);
	textureColor = texture_diffuse.Sample(SampleType, input.tex);
	return textureColor;
}
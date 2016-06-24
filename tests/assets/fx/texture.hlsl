// GLOBALS //
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

Texture2D diffuse;
SamplerState SampleType;

// TYPEDEFS //

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


// VertexShader
//------------------------------------------------------------------------------------
PixelInputType TextureVertexShader(VertexInputType input)
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
float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor = float4(0,0,0,1);
	textureColor = diffuse.Sample(SampleType, input.tex);
	//textureColor.x = 0;
	//textureColor.w = 1;
	return textureColor;
}
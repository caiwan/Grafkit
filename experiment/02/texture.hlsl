
// GLOBALS //
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

Texture2D shaderTexture;
SamplerState SampleType;

// TYPEDEFS //

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};


// VertexShader
//------------------------------------------------------------------------------------
PixelInputType TextureVertexShader(VertexInputType input)
{
	PixelInputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

	return output;
}

// PixelShader
//------------------------------------------------------------------------------------
float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;

	textureColor = shaderTexture.Sample(SampleType, input.tex);

	//textureColor.xy = input.tex;

	return textureColor;
}
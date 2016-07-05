
// GLOBALS //

Texture2D effectInput;
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
PixelInputType FullscreenQuad(VertexInputType input)
{
	PixelInputType output;
	input.position.w = 1.0f;
	output.position = input.position;
	output.tex = input.tex;
	return output;
}

// PixelShader
//------------------------------------------------------------------------------------
float4 CopyScreen(PixelInputType input) : SV_TARGET
{
	float4 textureColor = float4(0,0,0,1);
	textureColor = effectInput.Sample(SampleType, input.tex);
	return textureColor;
}
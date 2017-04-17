
// GLOBALS //
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

SamplerState SampleType {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

Texture2D t_diffuse;
Texture2D t_alpha;
Texture2D t_normal;
Texture2D t_specular;
Texture2D t_shininess;


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
	output.normal = input.normal;
	output.view = input.view;
	output.diff = t_diffuse.Sample(SampleType, input.tex);
	return output;
}

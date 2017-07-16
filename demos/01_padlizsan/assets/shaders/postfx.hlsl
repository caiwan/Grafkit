/*
    Basic passthrough shader
*/

Texture2D effectInput;
SamplerState SampleType;

struct PixelInputType
{
	float4 position : SV_POSITION; 
	float2 tex : TEXCOORD;
};

float4 passthrough(PixelInputType input) : SV_TARGET
{
	return effectInput.Sample(SampleType, input.tex);
}

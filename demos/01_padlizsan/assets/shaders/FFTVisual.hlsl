
Texture1D<float> fftTex;
SamplerState SampleType;

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 fftVisual(PixelInputType input) : SV_TARGET
{
	float f = fftTex.Sample(SampleType, input.tex.x); 
	float4 color = float4(f, f, f, 1);
	return color;
}

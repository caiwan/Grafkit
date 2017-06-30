

cbuffer FFT{
	float fftData[64];
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 fftVisual(PixelInputType input) : SV_TARGET
{
	float4 color = float4(1,0,0,1);

	return color;
}

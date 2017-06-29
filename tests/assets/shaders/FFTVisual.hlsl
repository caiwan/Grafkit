

cbuffer FFT{
	float fftData[64];
};

float4 fisheyeProc(PixelInputType input) : SV_TARGET
{
	float4 color = flaot4(1,0,0,1);

	return color;
}

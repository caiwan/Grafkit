#include <types.hlsl>


Texture2D effectInput;
SamplerState SampleType;

float4 CopyScreen(FXPixelInputType input) : SV_TARGET
{
    float4 textureColor = float4(0, 0, 0, 1);
    textureColor = effectInput.Sample(SampleType, input.tex);
    return textureColor;
}


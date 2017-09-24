#include <types.hlsl>
#include <common.hlsl>

#define MAP_INPUT 0
#define MAP_FRONT 1

#define MAP_VIEW 2
#define MAP_NORMAL 3
#define MAP_VELOCITY 4
#define MAP_EMISSION 5

cbuffer MatrixBuffer
{
    WorldMatrices matrices;
}

cbuffer EffectParams
{
    EffectParamType fxParams;
}

cbuffer TroubleshootParams
{
    int showOutput;
    int showMap;
}

Texture2D frontBuffer;
Texture2D effectInput;
Texture2D<float4> normalMap;
Texture2D<float4> viewMap;

SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 debugFx(FXPixelInputType input) : SV_TARGET
{
    float2 uv = input.position.xy / fxParams.screen.xy;
    float4 color = float4(0, 0, 0, 1);

    if (showOutput == MAP_FRONT)
        color = frontBuffer.Sample(SampleType, uv);

	// shown in full screen
    if (showOutput == MAP_FRONT)
        color = frontBuffer.Sample(SampleType, uv);
    else
        color = effectInput.Sample(SampleType, uv);
    
	// shown in thumbnail
    if (showMap != MAP_INPUT)
    {
        float2 uvThumb = input.position / fxParams.screen.xy * 3;
        float maskThumb = (uvThumb.x > 1 || uvThumb.y > 1.) ? 0. : 1.;
        uvThumb = saturate(uvThumb);

        float4 thumb = float4(0, 0, 0, 1);

        if (showMap == MAP_FRONT)
        {
            thumb = frontBuffer.Sample(SampleType, uvThumb);
        }
        else if (showMap == MAP_VIEW)
        {
            thumb = viewMap.Sample(SampleType, uvThumb);
        }
        else if (showMap == MAP_NORMAL)
        {
            thumb = .5 + normalMap.Sample(SampleType, uvThumb) * .5;
        }

        //else if (showOutput == MAP_VELOCITY)
        //    thumb = frontBuffer.Sample(SampleType, uv);

        //else if (showOutput == MAP_EMISSION)
        //    thumb = frontBuffer.Sample(SampleType, uv);
        else
        {
            thumb = effectInput.Sample(SampleType, uvThumb);
        }

        thumb.a = 1;
        color = lerp(color, thumb, maskThumb);

    }

    color.a = 1;

    return color;
}

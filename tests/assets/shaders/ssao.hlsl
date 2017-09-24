#include <types.hlsl>
#include <common.hlsl>

cbuffer MatrixBuffer
{
    WorldMatrices matrices;
}

cbuffer EffectParams
{
    EffectParamType fxParams;
}

cbuffer ssaoParamBuffer
{
    float4 ssaoParams;
}

Texture2D<float4> normalMap;
Texture2D<float4> viewMap;
Texture2D frontBuffer;
Texture2D effectInput;

Texture2D noiseMap;

static const int MAX_ITERATIONS = 16;

cbuffer ssaoKernelBuffer
{
    float4 kernels[128];
};

SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 SSAO(FXPixelInputType input) : SV_TARGET
{
    float4 d;
    d.w = 1;

    float2 uv = input.position.xy / fxParams.screen.xy;

    float4 pos = viewMap.Sample(SampleType, uv);
    float4 normal = normalMap.Sample(SampleType, uv);
    normal.w = 0;
    float ao = 0;

   // depth is usually null where ther is no depth information 
    if (length(pos.z) > 0.0001)
    {
        float2 noiseScale = ssaoParams.xy;
        float sampleRadius = ssaoParams.z;
        float rangeCheck = ssaoParams.w;

        normal = normalize(normal);

        float3 noise = noiseMap.Sample(SampleType, uv * noiseScale).xyz;

        float3 rvec = normalize(noise * 2.0 - 1.0);
        float3 N = normal.xyz;
        float3 T = normalize(rvec - N * dot(rvec, N));
        float3 B = cross(N, T);

        matrix tbn = matrix(float4(T, 0), float4(B, 0), float4(N, 0), float4(0, 0, 0, 1));
		
        for (int i = 0; i < MAX_ITERATIONS; i++)
        {
            float4 rn = normalize(kernels[i]);
            
            float scale = float(i) / float(MAX_ITERATIONS);
            scale = lerp(0.1f, 1.0f, scale * scale);
            rn *= scale;

            float4 samplepos = mul(rn, tbn);

            samplepos.xyz = samplepos * sampleRadius + pos;
            samplepos.w = 1.;

            float4 offset = samplepos;
            offset = mul(offset, matrices.projectionMatrix);
            offset.xy /= offset.w;
            offset.xy = offset.xy * 0.5 + 0.5;
            offset.y = 1. - offset.y;

            float d = viewMap.Sample(SampleType, offset.xy).z;

            float rangeCheck = abs(pos.z - d) < sampleRadius ? 1.0 : 0.0;
            ao += (d <= samplepos.z ? 1.0 : 0.0) * rangeCheck;
        }

        ao /= MAX_ITERATIONS;
    }

    ao = 1. - ao;

    return float4(ao, ao, ao, 1);
}


float4 SSAOSmooth(FXPixelInputType input) : SV_TARGET
{
    float4 color;
	
    float2 uv = input.position / fxParams.screen.xy;

	// fast denoise 
    float4 center = effectInput.Sample(SampleType, uv);
    float total = 0.0;
    for (float x = -4.0; x <= 4.0; x += 1.0)
    {
        for (float y = -4.0; y <= 4.0; y += 1.0)
        {
            float2 offset = float2(x, y) / fxParams.screen.xy;
            float4 sample = effectInput.Sample(SampleType, uv + offset);
            float weight = 1.0 - abs(dot(sample.rgb - center.rgb, float3(.25, .25, .25)));
            weight = pow(weight, 5);
            color += sample * weight;
            total += weight;
        }
    }

    color = color / total;
    color *= frontBuffer.Sample(SampleType, uv);

    return color;
}
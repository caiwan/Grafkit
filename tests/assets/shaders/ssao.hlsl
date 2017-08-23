#include <types.hlsl>
#include <common.hlsl>

cbuffer MatrixBuffer {
	WorldMatrices matrices;
}

Texture2D<float4> normalMapTexture;
Texture2D<float4> viewMapTexture;
Texture2D effectInput;

Texture2D noiseMap;

static const int MAX_ITERATIONS = 16; 

cbuffer ssaoParamBuffer{
	float4 ssaoParams;
}

cbuffer ssaoKernelBuffer {
	float4 kernels[128];
};

SamplerState SampleType {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

float4 SSAO(FXPixelInputType input) : SV_TARGET
{
	float4 pos = viewMapTexture.Sample(SampleType, input.tex);
	float4 normal = normalMapTexture.Sample(SampleType, input.tex);
	float ao = 0;

	// depth is usually null where ther is no depth information 
	if (length(pos.x) > 0.0001) 
	{
		float2 noiseScale = ssaoParams.xy;
		float sampleRadius = ssaoParams.z;
		float rangeCheck = ssaoParams.w;

		normal = normalize(normal);

		float3 rvec = normalize(noiseMap.Sample(SampleType, input.tex * noiseScale).xyz * 2.0 - 1.0);
		float3 tangent = normalize(rvec - normal.xyz * dot(rvec, normal.xyz));
		float3 bitangent = cross(normal, tangent);
		matrix tbn = mat3row(tangent, bitangent, normal);

		for (int i = 0; i < MAX_ITERATIONS; i++) {
			float4 rn = kernels[i];

			float scale = float(i) / float(MAX_ITERATIONS);
			scale = lerp(0.1f, 1.0f, scale * scale);
			rn *= scale;

			float4 samplepos = mul(rn, tbn);
			samplepos = samplepos * sampleRadius + pos;
			samplepos.w = 1.;

			float4 offset = samplepos;
			offset = mul(offset, matrices.projectionMatrix);
			offset.xy /= offset.w;
			offset.xy = offset.xy *0.5 + 0.5;
			offset.y = 1. - offset.y;

			float d = viewMapTexture.Sample(SampleType, offset).z;

			float rangeCheck = abs(pos.z - d) < sampleRadius ? 1.0 : 0.0;
			ao += (d <= samplepos.z ? 1.0 : 0.0) * rangeCheck;
		}

		ao /= MAX_ITERATIONS;
	}

	ao = 1. - ao;

	return float4(ao, ao, ao, 1);
}
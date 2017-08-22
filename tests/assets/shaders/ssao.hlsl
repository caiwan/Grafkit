#include <types.hlsl>
#include <common.hlsl>

cbuffer MatrixBuffer {
	WorldMatrices matrices;
}

Texture2D<float4> normalMapTexture;
Texture2D<float4> viewMapTexture;
Texture2D effectInput;

Texture2D noiseMap;


static const int MAX_ITERATIONS = 8; 

cbuffer ssaoParams{
	float2 noiseScale;
	float sampleRasius;
	float rangeCheck;
}

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

	if (length(normal) > 0.001) {
		normal = normalize(normal);
		float3 rvec = normalize(noiseMap.Sample(SampleType, input.tex * noiseScale) * 2.0 - 1.0);
		float3 tangent = normalize(rvec - normal * dot(rvec, normal));
		float3 bitangent = cross(normal, tangent);
		matrix tbn = matrix(tangent, bitangent, normal);

		for (int i = 0; i < MAX_ITERATIONS; i++) {
			float3 rn = normalize(noise3(input.tex));
			rn.z = abs(rn.z);
			float scale = float(i) / float(MAX_ITERATIONS);
			scale = lerp(0.1f, 1.0f, scale * scale);
			rn *= scale;

			float3 samplepos = tbn * rn;
			samplepos = samplepos * sampleRadius + pos;

			vec4 offset = vec4(sampleuv, 1.0);
			offset = mul(offset, matrices.projectionMatrix);
			offset.xy /= offset.w;
			offset.xy = offset.xy * 0.5 + 0.5;

			float d = viewMapTexture.Sample(SampleType, offset).z;

			float rangeCheck = abs(origin.z - sampleDepth) < sampleRadius ? 1.0 : 0.0;
			ao += (d <= sample.z ? 1.0 : 0.0) * rangeCheck;
		}

		ao /= MAX_ITERATIONS;
	}

	ao = 1. - ao;

	return float3(ao, ao, ao, 1);
}
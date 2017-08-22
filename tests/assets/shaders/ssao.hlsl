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

cbuffer ssaoParamBuffer{
	float4 ssaoParams;
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

	//if (length(normal) > 0.0001) 
	{
		float2 noiseScale = ssaoParams.xy;
		float sampleRadius = ssaoParams.z;
		float rangeCheck = ssaoParams.w;

		normal = normalize(normal);

		float3 rvec = normalize(noiseMap.Sample(SampleType, input.tex * noiseScale).xyz * 2.0 - 1.0);
		float3 tangent = normalize(rvec - normal * dot(rvec, normal.xyz));
		float3 bitangent = cross(normal, tangent);
		matrix tbn = matrix (
			tangent.x, bitangent.x, normal.x, 0,
			tangent.y, bitangent.y, normal.y, 0,
			tangent.z, bitangent.z, normal.z, 0,
			0, 0, 0, 1
		);

		for (int i = 0; i < MAX_ITERATIONS; i++) {
			float4 rn;
			rn.w = 1.;
				rn.xyz = normalize(noise3(input.tex));
			rn.z = abs(rn.z);
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
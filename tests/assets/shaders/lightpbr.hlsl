#include <types.hlsl>

SamplerState SampleType {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

Texture2D t_diffuse;
Texture2D t_alpha;
Texture2D t_normal;
Texture2D t_metallic;
Texture2D t_roughness;
Texture2D t_emission;
TextureCube c_specular;

float3x3 cotangent_frame(float3 N, float3 p, float2 uv)
{
    // get edge vectors of the pixel triangle
    float3 dp1 = ddx(p);
    float3 dp2 = ddy(p);
    float2 duv1 = ddx(uv);
    float2 duv2 = ddy(uv);

    // solve the linear system
    float3 dp2perp = cross(dp2, N);
    float3 dp1perp = cross(N, dp1);
    float3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    float3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
    // construct a scale-invariant frame 
    float invmax = rsqrt(max(dot(T, T), dot(B, B)));
    return float3x3(T * invmax, B * invmax, N);
}

float3 perturb_normal(float3 N, float3 V, float3 map, float2 texcoord)
{
    // assume N, the interpolated vertex normal and 
    // V, the view vector (vertex to eye)
    map = map * 2 - 1;
    //map = float3(0.0f, 0.0f, 1.0f); // Flat normal.
    float3x3 TBN = cotangent_frame(N, -V, texcoord);
    return normalize(mul(map, TBN));
}

// PixelShader
//------------------------------------------------------------------------------------

PixelOutType mainPixel(PixelInputType input)
{
	PixelOutType output;

	float3 color;

	float3 surface_normal = normalize(input.normal.xyz);
    float3 ray_view = input.view;
	float3 ray_dir = normalize(input.view);
    float3 reflect_normal = reflect(ray_dir, surface_normal);

	float fresnel = 1.0 + dot(surface_normal, ray_dir);
	fresnel *= fresnel;

	float4 diffuse_color = t_diffuse.Sample(SampleType, input.tex);
	float4 emission_color = t_emission.Sample(SampleType, input.tex);
    float3 normal_sample = t_normal.Sample(SampleType, input.tex);
    surface_normal = perturb_normal(surface_normal, ray_view, normal_sample, input.tex.xy);
	
    color = emission_color.rgb + diffuse_color.rgb;

    output.diff.rgb = saturate(color);
    output.diff.a = diffuse_color.a;
            
	output.normal.xyz = surface_normal;
	output.view = input.view;
    output.emission = emission_color;
	return output;
}

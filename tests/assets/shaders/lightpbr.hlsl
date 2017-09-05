#include <types.hlsl>

// Heavily based on http://orsvarn.com/post/pbr-shader-source/
// https://gist.github.com/caiwan/75aa59f307db4e330c4e4d6d818943d8
// and Mu6k's stuff 

SamplerState SampleType {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

cbuffer LightBuffer
{
    Light lights[8];
    int lightCount;
};

cbuffer EnvironmentBuffer
{
    float globalTime;
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
TextureCube c_irradiance;

// Constants
static const float Pi = PI;
static const float OneDivPi = 1 / PI;

// Lys constants
static const float k0 = 0.00098, k1 = 0.9921, fUserMaxSPow = 0.2425;
static const float g_fMaxT = (exp2(-10.0 / sqrt(fUserMaxSPow)) - k0) / k1;
static const int nMipOffset = 0;

// Lys function, copy/pasted from https://www.knaldtech.com/docs/doku.php?id=specular_lys
float GetSpecPowToMip(float fSpecPow, int nMips)
{
    // This line was added because ShaderTool destroys mipmaps.
    fSpecPow = 1 - pow(1 - fSpecPow, 8);
    // Default curve - Inverse of Toolbag 2 curve with adjusted constants.
    float fSmulMaxT = (exp2(-10.0 / sqrt(fSpecPow)) - k0) / k1;
    return float(nMips - 1 - nMipOffset) * (1.0 - clamp(fSmulMaxT / g_fMaxT, 0.0, 1.0));
}

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

// TODO: make normals flat
float3 perturb_normal(float3 N, float3 V, float3 map, float2 texcoord)
{
    // assume N, the interpolated vertex normal and 
    // V, the view vector (vertex to eye)
    map = map * 2 - 1;
    //map = float3(0.0f, 0.0f, 1.0f); // Flat normal.
    float3x3 TBN = cotangent_frame(N, -V, texcoord);
    return normalize(mul(map, TBN));
}

float hash(float2 t, float3 c)
{
    float s = dot(frac(t.xyx * 1132.914), frac(c.xyz * 0.495));
    s = frac(s * 45.723 + t.x * t.y * 11.951312 + 11.8231);
    s = frac(s * 11.423 + s * t.y * 511.451312 + 1.5231);
    s = frac(s * 11.423 + t.x * s * 511.451312 + 1.5231);
    return s;
}

// PixelShader
//------------------------------------------------------------------------------------

PixelOutType mainPixel(PixelInputType input)
{
	PixelOutType output;

	float3 surface_normal = normalize(input.normal.xyz);
    float3 ray_view = input.view;
	float3 ray_dir = normalize(input.view);
    float3 reflect_normal = reflect(ray_dir, surface_normal);

	float fresnel = 1.0 + dot(surface_normal, ray_dir);
	fresnel *= fresnel;

	// 
    int mipLevels, width, height;
    c_specular.GetDimensions(0, width, height, mipLevels);

	float4 base_color = t_diffuse.Sample(SampleType, input.tex);
	float4 emission_color = t_emission.Sample(SampleType, input.tex);
    float3 normal_sample = t_normal.Sample(SampleType, input.tex);

    float metallic_sample = t_metallic.Sample(SampleType, input.tex);
    float roughness_sample = t_roughness.Sample(SampleType, input.tex);

    float4 cubemap_sampleAmbient = c_specular.Sample(SampleType, normal_sample);
    float4 cubemap_sampleSpec = c_specular.SampleLevel(SampleType, reflect_normal, GetSpecPowToMip(roughness_sample, mipLevels));

    roughness_sample = pow(roughness_sample, 0.4);

    surface_normal = perturb_normal(surface_normal, ray_view, normal_sample, input.tex.xy);
	// 
	// Creating specular color and intensity, this needs to be done before gamma correction
    float4 specular_color = float4(lerp(0.04f.rrr, base_color.rgb, metallic_sample), 1.0f);
    base_color.rgb = lerp(base_color.rgb, 0.0f.rrr, metallic_sample);

    // Gamma correct textures
    base_color = pow(abs(base_color), 2.2);
    roughness_sample = pow(abs(roughness_sample), 2.2);
    cubemap_sampleAmbient = pow(abs(cubemap_sampleAmbient), 2.2);
    cubemap_sampleSpec = pow(abs(cubemap_sampleSpec), 2.2);
	 
    // Material settings
    float roughPow2 = roughness_sample * roughness_sample;
    float roughPow22 = roughPow2 * roughPow2;
    float roughPow2Half = roughPow2 * 0.5;

    // Directional lights
    float4 diffuse_light = 0;
    float4 specular_light = 0;
    for (int i = 0; i < 1; i++)
    {
        float3 light_dir = normalize(lights[i].position.xyz);
        float3 light_color = lights[i].color.rgb;
        float light_intensity = lights[i].param1.a * 10000;
      
        // DIFFUSE
        diffuse_light += float4((saturate(dot(-light_dir, normal_sample)) * OneDivPi) * light_color * light_intensity * base_color.rgb, 1.0f);
                
		// SPECULAR 
        float3 halfVector = normalize(ray_view + light_dir);
        float halfVecDotNorm = dot(halfVector, normal_sample);
        float normDotCam = max(dot(normal_sample, -ray_view), 0);
        float normDotLight = max(dot(normal_sample, -light_dir), 0);

        // Fresnel term
        float4 schlick_fresnel = specular_color + (1 - specular_color) * (pow(1 - dot(ray_view, halfVector), 5) / (6 - 5 * (1 - roughness_sample)));
        
        // Distribution term
        float denominator = halfVecDotNorm * halfVecDotNorm * (roughPow22 - 1) + 1;
        float ggxDistribution = roughPow22 / (Pi * denominator * denominator);
        
        // Geometry term
        float schlickGgxGeometry = (normDotCam / (normDotCam * (1 - roughPow2Half) + roughPow2Half));
        //float ggxGeometry = (2*normDotCam) / (normDotCam + sqrt(roughPow2 +
        //       (1 - roughPow2)*pow(normDotCam, 2))); // ggxG without schlick approximation

        // Add the spec from this light
        specular_light += float4(((schlick_fresnel * ggxDistribution * schlickGgxGeometry) / 4 * normDotLight * normDotCam).rrr * light_color * specular_color.rgb * light_intensity, 1.0f);
    }
  
    // Ambient cubemap light
    diffuse_light.rgb += cubemap_sampleAmbient.rgb * base_color.rgb;
 
    // Specular cubemap light 
    float normDotCam = max(dot(lerp(input.normal, normal_sample, max(dot(input.normal, -ray_view), 0)), -ray_view), 0);
    float4 schlick_fresnel = saturate(specular_color + (1 - specular_color) * pow(1 - normDotCam, 5));

	 // Ambient cubemap light
    diffuse_light.rgb += cubemap_sampleAmbient.rgb * base_color.rgb;

    // Composite
    float4 color;
    color.rgb = metallic_sample;
    color.a = 1.;
    //color = lerp(diffuse_light, cubemap_sampleSpec, schlick_fresnel);
    //color += specular_light;

    float noise = hash(input.position * 0.01, color);

    color = pow(abs(color), .4545);
    color.a = base_color.a;

    color += emission_color;

    color += noise / 256.0;
	 
    output.diff = color;
	output.normal.xyz = surface_normal;
	output.view = input.view;
    output.emission = emission_color;
	return output;
}

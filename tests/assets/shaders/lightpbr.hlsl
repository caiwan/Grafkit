#include <types.hlsl>

// Globals
cbuffer MatrixBuffer {
	WorldMatrices matrices;
}


// Material and shader model has to be rewritten completely to fit into PBR standards

// Lights
cbuffer Lights {
	float DirectionalLightCount;
	float4 DirectionalLightColor[8];
	float4 DirectionalLightDirection[8];
}
// Textures

Texture2D TexBaseColor;
Texture2D TexNormal;

cbuffer material_params
{
	float4 mat_param0, mat_param1, mat_param2;
};

Texture2D TexRoughness;
Texture2D TexMetallic;
Texture2D TexOcclusion;

TextureCube TexCubemapIrradiance;
TextureCube TexCubemapSpecular;

SamplerState TextureSampler
{
    //Filter = MIN_MAG_MIP_Linear;
    Filter = ANISOTROPIC;
    AddressU = WRAP;
    AddressV = WRAP;
};

// Constants
static const float OneDivPi = 1/PI;

// Lys constants
static const float k0 = 0.00098, k1 = 0.9921, fUserMaxSPow = 0.2425;
static const float g_fMaxT = ( exp2(-10.0/sqrt( fUserMaxSPow )) - k0)/k1;
static const int nMipOffset = 0;

// Lys function, copy/pasted from https://www.knaldtech.com/docs/doku.php?id=specular_lys
float GetSpecPowToMip(float fSpecPow, int nMips)
{
    // This line was added because ShaderTool destroys mipmaps.
    fSpecPow = 1 - pow(1 - fSpecPow, 8);
    // Default curve - Inverse of Toolbag 2 curve with adjusted constants.
    float fSmulMaxT = ( exp2(-10.0/sqrt( fSpecPow )) - k0)/k1;
    return float(nMips-1-nMipOffset)*(1.0 - clamp( fSmulMaxT/g_fMaxT, 0.0, 1.0 ));
}

float3x3 cotangent_frame( float3 N, float3 p, float2 uv )
{
    // get edge vectors of the pixel triangle
    float3 dp1 = ddx( p );
    float3 dp2 = ddy( p );
    float2 duv1 = ddx( uv );
    float2 duv2 = ddy( uv );

    // solve the linear system
    float3 dp2perp = cross( dp2, N );
    float3 dp1perp = cross( N, dp1 );
    float3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    float3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
    // construct a scale-invariant frame 
    float invmax = rsqrt( max( dot(T,T), dot(B,B) ) );
    return float3x3( T * invmax, B * invmax, N );
}

float3 perturb_normal( float3 N, float3 V, float2 texcoord )
{
    // assume N, the interpolated vertex normal and 
    // V, the view vector (vertex to eye)
    float3 map = TexNormal.Sample( TextureSampler, texcoord ).xyz;
    map = map*2-1;
    //map = float3(0.0f, 0.0f, 1.0f); // Flat normal.
    float3x3 TBN = cotangent_frame( N, -V, texcoord );
    return normalize( mul(map, TBN) );
}

// Pixel Shader
float4 PS_Main(PixelInputType input) : SV_TARGET
{
    float3 camToPixelRaw = input.view;
    float3 camToPixel = normalize(camToPixelRaw);

    // Calculate normal
    float3 normal = perturb_normal(normalize(input.Normal), -camToPixelRaw, input.UV);
    float3 camNormalReflect = reflect(camToPixel, normal);

    // Sample textures
    int mipLevels, width, height;
    TexCubemapSpecular.GetDimensions(0, width, height, mipLevels);
    float4 baseColorSample = TexBaseColor.Sample(TextureSampler, input.UV);
    float4 normalSample = TexNormal.Sample(TextureSampler, input.UV);
    float roughnessSample = abs(TexRoughness.Sample(TextureSampler, input.UV).r);
    float metallicSample = TexMetallic.Sample(TextureSampler, input.UV).r;
    float occlusionSample = TexOcclusion.Sample(TextureSampler, input.UV).r;

    float4 cubemapSampleAmbient = TexCubemapIrradiance.Sample(TextureSampler, normal);
    float4 cubemapSampleSpec = TexCubemapSpecular.SampleLevel(TextureSampler, camNormalReflect,
                                 GetSpecPowToMip(roughnessSample, mipLevels));
    roughnessSample = pow(roughnessSample, 0.4);

    // Creating specular color and intensity, this needs to be done before gamma correction
    float4 specularColor = float4(lerp(0.04f.rrr, baseColorSample.rgb, metallicSample), 1.0f);
    baseColorSample.rgb = lerp(baseColorSample.rgb, 0.0f.rrr, metallicSample);

    // Gamma correct textures
    baseColorSample = pow(abs(baseColorSample), 2.2);
    roughnessSample = pow(abs(roughnessSample), 2.2);
    cubemapSampleAmbient = pow(abs(cubemapSampleAmbient), 2.2);
    cubemapSampleSpec = pow(abs(cubemapSampleSpec), 2.2);

    // Add in occlusion
    float occlusion = 1 - (1 - occlusionSample)*0.75f;
    cubemapSampleAmbient *= occlusion;
    cubemapSampleSpec *= occlusion;

    // Material settings
    float roughPow2 = roughnessSample*roughnessSample;
    float roughPow22 = roughPow2*roughPow2;
    float roughPow2Half = roughPow2*0.5;

    // Directional lights
    float4 diffuseLight = 0;
    float4 specularLight = 0;
    for(int i = 0; i < DirectionalLightCount; i++)
    {
        float3 lightDir = normalize(DirectionalLightDirection[i].xyz);
        float3 lightColor = DirectionalLightColor[i].rgb;
        float lightIntensity = DirectionalLightColor[i].a*10000;
      
        // DIFFUSE
        diffuseLight += float4((saturate(dot(-lightDir, normal))*OneDivPi)*
                        lightColor*lightIntensity*baseColorSample.rgb, 1.0f);
        
        // SPECULAR
        float3 halfVector = normalize(camToPixel + lightDir);
        float halfVecDotNorm = dot(halfVector, normal);
        float normDotLight = max(dot(normal, -lightDir), 0);
        float normDotCam = max(dot(normal, -camToPixel), 0);
        
        // Fresnel term
        float4 schlickFresnel = specularColor + (1 - specularColor) *
                                (pow(1 - dot(camToPixel, halfVector), 5) /
                                (6 - 5*(1-roughnessSample)));
        
        // Distribution term
        float denominator = halfVecDotNorm*halfVecDotNorm*(roughPow22 - 1) + 1;
        float ggxDistribution = roughPow22/(Pi*denominator*denominator);
        
        // Geometry term
        float schlickGgxGeometry = (normDotCam / (normDotCam*(1 - roughPow2Half) + roughPow2Half));
        //float ggxGeometry = (2*normDotCam) / (normDotCam + sqrt(roughPow2 +
        //       (1 - roughPow2)*pow(normDotCam, 2))); // ggxG without schlick approximation

        // Add the spec from this light
        specularLight += float4(((schlickFresnel*ggxDistribution*schlickGgxGeometry) /
                         4*normDotLight*normDotCam).rrr *
                         lightColor*specularColor.rgb*lightIntensity, 1.0f);
  }
  
    // Ambient cubemap light
    diffuseLight.rgb += cubemapSampleAmbient.rgb*baseColorSample.rgb;

    // Specular cubemap light
    float normDotCam = max(dot(lerp(input.Normal, normal, max(dot(input.Normal, -camToPixel), 0)),
                        -camToPixel), 0);
    float4 schlickFresnel = saturate(specularColor + (1 - specularColor)*pow(1 - normDotCam, 5));

    // Composite
    float4 output = 0;
    output = lerp(diffuseLight, cubemapSampleSpec, schlickFresnel);
    output += specularLight;

    output = pow(abs(output), .4545);
    output.a = 1;
    return output;
}

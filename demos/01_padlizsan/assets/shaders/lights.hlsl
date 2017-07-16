cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct Light
 {
 	int type;
 	float4 position;
 	float4 direction;

 	float4 ambient;
 	float4 diffuse;
 	float4 specular;

 	float ca, la, qa;

 	float angle, falloff;
 };

cbuffer material
{
	// struct Material
	int material_type;
	float4 material_ambient, material_diffuse, material_specular, material_emission;
	float material_specularLevel;
	float material_shininess;
    
    float material_intensity;
    float material_roughness;
};

 cbuffer light
 {
 	int is_lightOn[4];
 	struct Light lights[4];
 }

struct VertexInputType
{
	float4 position : POSITION;
	float4 normal :	NORMAL;
	float4 tangent : TANGENT;
	float4 binormal : BINORMAL;
	float2 tex : TEXCOORD;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 binormal : BINORMAL;
	
	float4 tex : TEXCOORD0;
	float4 worldPosition : TEXCOORD1;
};

SamplerState SampleType {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

Texture2D t_diffuse;
Texture2D t_alpha;
Texture2D t_normal;
Texture2D t_specular;
Texture2D t_shininess;

PixelInputType mainVertex(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;

	output.worldPosition = input.position;
	output.worldPosition = mul(output.worldPosition, worldMatrix);
	output.worldPosition = mul(output.worldPosition, viewMatrix);
	output.worldPosition.xyz = output.worldPosition.xyz / output.worldPosition.w;

	output.position = input.position;
	output.position = mul(output.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	input.normal.w = 0.0f;
	output.normal = input.normal;
	output.normal = mul(output.normal, worldMatrix);
	output.normal = mul(output.normal, viewMatrix);
	output.normal = mul(output.normal, projectionMatrix);
	output.normal = normalize(output.normal);

	input.tangent.w = 0.0f;
	output.tangent = input.tangent;
	output.tangent = mul(output.tangent, worldMatrix);
	output.tangent = mul(output.tangent, viewMatrix);
	output.tangent = mul(output.tangent, projectionMatrix);
	output.tangent = normalize(output.tangent);

	input.binormal.w = 0.0f;
	output.binormal = input.binormal;
	output.binormal = mul(output.binormal, worldMatrix);
	output.binormal = mul(output.binormal, viewMatrix);
	output.binormal = mul(output.binormal, projectionMatrix);
	output.binormal = normalize(output.binormal);

	output.tex.zw = float2(0.0f, 1.0f);
	output.tex.xy = input.tex.xy;

	return output;
}

float orenNayarDiffuse(
  float3 lightDirection,
  float3 viewDirection,
  float3 surfaceNormal,
  float roughness,
  float intensity
)
{
  
  float LdotV = dot(lightDirection, viewDirection);
  float NdotL = dot(lightDirection, surfaceNormal);
  float NdotV = dot(surfaceNormal, viewDirection);

  float s = LdotV - NdotL * NdotV;
  float t = lerp(1.0, max(NdotL, NdotV), step(0.0, s));

  float sigma2 = roughness * roughness;
  float A = 1.0 + sigma2 * (intensity / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
  float B = 0.45 * sigma2 / (sigma2 + 0.09);

  return intensity * max(0.0, NdotL) * (A + B * s / t) / PI;
}

//------------------------------------------------------------------------------------

float4 mainPixel(PixelInputType input) : SV_TARGET
{
	float4 color = material_ambient;
    
    float ep = float4(0,0,0,1);
    
    
    float3 lightPosition = lights[0].position;
    float3 surfacePosition = input.worldPosition;
    float3 eyePosition = 
    
    float3 lightDirection = normalize(lightPosition - surfacePosition);
    float3 viewDirection = normalize(eyePosition - surfacePosition);

  //Surface properties
  vec3 normal = normalize(surfaceNormal);
	float l = orenNayarDiffuse(
       
    );
    	
	return color;
}
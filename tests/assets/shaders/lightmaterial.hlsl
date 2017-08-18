#include <types.hlsl>

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

	float4 param1, param2;
};

cbuffer light
{
	struct Light light;
}

cbuffer material_params
{
	float4 mat_param0, mat_param1, mat_param2;
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


// PixelShader
//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------

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
// Sttic Phong-Blinn model
//------------------------------------------------------------------------------------

struct PixelOutType phongBlinn(PixelInputType input)
{
	PixelOutType output;

	output.normal = input.normal;
	output.view = input.view;

	float4 m, s;
	m.rgb = input.color0.rgb;
	s.rgb = input.color1.rgb;
	m.a = 1;
	s.a = 1;

	float f = mat_param0.x;

	float3 p = input.view.xyz;
	float3 lp = light.position.xyz;
	float3 mp = lp - p;

	float3 N = input.normal.xyz;
	float3 E = normalize(-p.xyz);
	float3 R = reflect(-mp, N.xyz);
	float3 L = normalize(mp);

	float lambda = saturate(dot(N, L));
	float theta = saturate(pow(saturate(dot(R, E)), f));

	output.diff.rgb = m.rgb * float3(lambda, lambda, lambda) + s.rgb * float3(lambda * theta, lambda * theta, lambda * theta);
	output.diff.a = m.a;

	return output;
}

//------------------------------------------------------------------------------------

struct PixelOutType phongBlinnTextured(PixelInputType input) : SV_TARGET
{
	PixelOutType output;

	output.normal = input.normal;
	output.view = input.view;

	float4 m;
	m.rgb = input.color0.rgb;
	m.a = 1;

	float3 p = input.view.xyz;
	float3 lp = light.position.xyz;
	float3 mp = lp - p;

	float3 N = input.normal.xyz;
	float3 E = normalize(-p.xyz);
	float3 R = reflect(-mp, N.xyz);
	float3 L = normalize(mp);

	float lambda = saturate(dot(N, L));
	//float theta = pow(saturate(dot(R, E)), f);

	output.diff.rgb = m.rgb * float3(lambda, lambda, lambda);
	output.diff.a = m.a;

	return output;
}

//------------------------------------------------------------------------------------

float4 orenNayar(PixelInputType input)
{
	//float4 color = material_ambient;

	//float ep = float4(0,0,0,1);

	//float3 lightPosition = lights[0].position;
	//float3 surfacePosition = input.worldPosition;
	//float3 eyePosition = viewMatrix[3];

	//float3 surfaceNormal = input.normal;

	//float3 lightDirection = normalize(lightPosition - surfacePosition);
	//float3 viewDirection = normalize(eyePosition - surfacePosition);

	//float roughness = material_roughness;
	//float intensity = material_intensity;

	////Surface properties
	//float3 normal = normalize(surfaceNormal);
	//float l = orenNayarDiffuse(
	//	lightDirection,
	//	viewDirection,
	//	surfaceNormal,
	//	roughness,
	//	intensity
	//);

	//color += material_diffuse * lights[0].diffuse * l;

	//return color;
}

//------------------------------------------------------------------------------------
struct PixelOutType orenNayarTextured(PixelInputType input) {

}

//------------------------------------------------------------------------------------

struct PixelOutType mainPixelDispatcher(PixelInputType input) {
	// this just dispatches between the defferent material layers
}


#define PI 3.14159265359

//cbuffer MatrixBuffer
//{
//	matrix worldMatrix;
//	matrix viewMatrix;
//	matrix projectionMatrix;
//};

/**
* http://www.rastertek.com/dx10tut06.html
* http://www.rastertek.com/dx10tut10.html
* http://www.rastertek.com/dx11tut30.html
*
*/

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
	float4 mat_param1, mat_param2;
};


struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 binormal : BINORMAL;

	float4 color0 : COLOR0;	// diffuse
	//float4 color1 : COLOR1;	// specular

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

// TODO::
// float CookTorrSpecular(){}
// float WardIsoSpecular(){}

//------------------------------------------------------------------------------------
// Sttic Phong-Blinn model
//------------------------------------------------------------------------------------

float4 mainPixel_PhongBlinn(PixelInputType input) : SV_TARGET
{
	float3 color = float3(0,0,0); 
	
	//float3 m = input.color0.rgb;
	float3 m = float3(1, 1, 1);

	float3 p = input.worldPosition.xyz;
	float3 lp = light.position.xyz;
	float3 mp = lp - p;

	float3 N = input.normal.xyz;
	float3 E = normalize(-p.xyz);
	float3 R = reflect(-mp, N.xyz);
	float3 L = normalize(mp);

	float lambda = saturate(dot(N, L));
	//float theta = pow(saturate(dot(R, E)), f);

	color = m * float3(lambda, lambda, lambda);

	return float4(color.r, color.g, color.b, 1);
}

//------------------------------------------------------------------------------------

float4 mainPixel_DiffuseTexture(PixelInputType input) : SV_TARGET
{
	//float4 color = material_ambient;

	//return color;
}

//------------------------------------------------------------------------------------

float4 mainPixel_OrenNayar(PixelInputType input) : SV_TARGET
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

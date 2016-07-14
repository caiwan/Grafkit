cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

/**
* http://www.rastertek.com/dx10tut06.html
* http://www.rastertek.com/dx10tut10.html
* http://www.rastertek.com/dx11tut30.html
*
*/

struct Material {
	int m_type;
	float4 m_ambient, m_diffuse, m_specular, m_emission;
	float m_specularLevel;
	float m_shininess;
} material;

struct Light
{
	int l_type;
	float4 l_position;
	float4 l_direction;

	float4 l_ambient;
	float4 l_diffuse;
	float4 l_specular;

	float l_ca, l_la, l_qa;

	float l_angle, l_falloff;
};

cbuffer material
{
	struct Material material;
};

cbuffer light
{
	int is_lightOn[4];
	struct Light lights[16];
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
Texture2D t_normal;
Texture2D t_specular;


PixelInputType mainVertex(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;

	output.worldPosition = input.position;
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

// PixelShader
//------------------------------------------------------------------------------------


//float4 calcPhongBlinn(float4 worldPosition, float4 texel, float4 normal, Material material, Light light)
//{
//	float4 p = light.l_position;
//	float3 d = normalize(worldPosition - p);
//	float lambda = saturate(dot(normal, -d)); /* lambert */
//	lambda *= (light.qa / dot(p - worldPosition, p - worldPosition));
//	
//	float3 h = normalize(normalize(CameraPos - worldPosition) - d);
//
//	float e = pow(saturate(dot(h, input.Normal)), material.m_shininess); /* specular */
//
//	return float4(saturate(
//		material.m_ambient +
//		(material.m_diffuse * light.l_diffuse * lambda * 0.6) + 
//		(material.m_specular * light.l_specular * e * 0.5) 
//	), 1);
//}

float4 mainPixel(PixelInputType input) : SV_TARGET
{
	float4 textureColor = float4(0,0,0,1);
	textureColor = t_diffuse.Sample(SampleType, input.tex);
	
	//float4 color0 = calcPointLight(input, material, lights[0]);

	return textureColor;
}

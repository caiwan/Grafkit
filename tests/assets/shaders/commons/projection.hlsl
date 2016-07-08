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
 */


cbuffer material
{
	struct {
		int m_type;
		float4 m_ambient, m_diffuse, m_specular, m_emission;
		float m_specularLevel;
		float m_shininess;
	} material;
};

cbuffer light
{
	int is_lightOn[4];

	struct
	{
		int l_type;
		float4 l_position;
		float4 l_direction;

		float4 l_ambient;
		float4 l_diffuse;
		float4 l_specular;

		float l_ca, l_la, l_qa;

		float l_angle, l_falloff;
	} lights[4];

}

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float binormal : BINORMAL;
	float2 tex : TEXCOORD;
	float4 lightPos1 : TEXCOORD1;
	float4 lightPos2 : TEXCOORD2;
	float4 lightPos3 : TEXCOORD3;
	float4 lightPos4 : TEXCOORD4;
};

struct VertexInputType
{
	float4 position : POSITION;
	float4 normal :	NORMAL;
	float4 tangent : TANGENT;
	float binormal : BINORMAL;
	float2 tex : TEXCOORD;
};

PixelInputType projectScene(VertexInputType input) {
	PixelInputType output;

	input.position.w = 1.0f;
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

	input.normal.w = 0.0f;
	output.binormal = input.binormal;
	output.binormal = mul(output.binormal, worldMatrix);
	output.binormal = mul(output.binormal, viewMatrix);
	output.binormal = mul(output.binormal, projectionMatrix);
	output.binormal = normalize(output.binormal);

	output.lightPos1.xyz = lights[0].l_position.xyz - output.position.xyz;
	output.lightPos2.xyz = lights[1].l_position.xyz - output.position.xyz;
	output.lightPos3.xyz = lights[2].l_position.xyz - output.position.xyz;
	output.lightPos4.xyz = lights[3].l_position.xyz - output.position.xyz;

	output.lightPos1 = normalize(output.lightPos1);
	output.lightPos2 = normalize(output.lightPos2);
	output.lightPos3 = normalize(output.lightPos3);
	output.lightPos4 = normalize(output.lightPos4);

	output.tex = input.tex;

	return output;
}

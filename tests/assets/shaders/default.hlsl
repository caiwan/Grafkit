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
	int type;
	float4 ambient, diffuse, specular, emission;
	float specularLevel;
	float shininess;

	int has_t_diffuse;		///< 1st map
	int has_t_alpha;		///< alpha map
	int has_t_normal;		///< bump map
	int has_t_shiniess; 	///< shininess map
	int has_t_specular;	///< specular map
	int has_t_selfillum;	///< self illumination map
	int has_t_reflect;		///< reflection map
	int has_t_bump;		///< bump map
	int has_t_aux0;		///< aux texture, used for pretty much everything else
	int has_t_aux1;		///< aux texture, used for pretty much everything else
	int has_t_aux2;		///< aux texture, used for pretty much everything else
	int has_t_aux3;		///< aux texture, used for pretty much everything else
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

// PixelShader
//------------------------------------------------------------------------------------

// regi glsl kod:
//float4 calcMaterial(Material material, Light light, MaterialPass mp, float3 normal) {
//	//if (light.isActive == 0.)
//	//	return vec4(0.);
//
//	float4 color = vec4(light.ambient.rgb, 0.);
//
//	// L nem jon at <- mp.dir
//
//	float3 E = normalize(fragEyeVector);
//	float3 N = normalize(normal);
//	float3 L = normalize(mp.dir); float d = length(mp.dir);
//	float3 R = normalize(mp.halfVector);
//
//	float lambda = dot(N, L);
//	float blinn = dot(R, E);
//
//	return vec4(float3(L / (L + 1.)), 1.);
//
//	if (lambda > 0.0)
//	{
//		float a = 1. / (light.ca + d* light.la + d* d * light.qa);
//		//float a = 1.;
//		color += vec4(light.diffuse.rgb, 1.) * lambda * a;
//		if (blinn>0. &&  material.shininess>0.) {
//			color += vec4(light.specular.rgb, 1.) * pow(blinn, material.shininess) * material.specularLevel * a;
//		}
//	}
//
//	return color;
//}

//MaterialPass calcMaterialToLight(in Light light, in vec3 normal, in vec3 pos) {
//	MaterialPass result;
//
//	vec4 lp = light.position - uModelView[3];
//
//	//result.dir = vec3(light.position.xyz - pos);
//	result.dir = vec3(lp.xyz - pos);
//	result.halfVector = reflect(-result.dir, normal);
//
//	return result;
//}

// forras?
//float4 calcPhongBlinn(float4 worldPosition, float4 texel, float4 normal, Material material, Light light)
//{
//	float4 p = light.position;
//	float3 d = normalize(worldPosition - p);
//	float lambda = saturate(dot(normal, -d)); /* lambert */
//	// lambda *= (light.qa / dot(p - worldPosition, p - worldPosition));
//	
//	float3 h = normalize(normalize(CameraPos - worldPosition) - d);
//	float e = pow(saturate(dot(h, input.Normal)), material.m_shininess); /* specular */
//
//	return float4(saturate(
//		material.m_ambient +
//		(material.m_diffuse * light.diffuse * lambda * 0.6) + 
//		(material.m_specular * light.specular * e * 0.5) 
//	), 1);
//}

//------------------------------------------------------------------------------------

float4 mainPixel(PixelInputType input) : SV_TARGET
{
	float4 color = float4(0,0,0,1);
	

	if (material.has_t_diffuse == 1)
		color = t_diffuse.Sample(SampleType, input.tex);
	else
		color = material.diffuse;

	// phong light
	if (material.type == 1) {
		float3 lp = float3(10, 10, 10);

		// proto
		float3 p = input.worldPosition.xyz;
		float3 mp = lp - p;
		float3 E = normalize(-p);
		float3 N = input.normal.xyz;
		float3 L = normalize(mp); float d = length(mp);
		// flaot3 

		float lambda = dot(N, L);
		// float theta = dot(R, E);

		// color = calcPointLight(input, material, lights[0]);
		color.xyz = color * float3(lambda, lambda, lambda);
		// color.xyz = .5 + N.xyz * .5; // normal szar, puszi
	}

	return color;
}

//------------------------------------------------------------------------------------

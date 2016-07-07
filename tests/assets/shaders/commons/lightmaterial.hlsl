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
	} lights[16];

}

struct LightMaterial {
	/**/
};


LightMaterial calculateLifght() {
	LightMaterial result;

	return result;
}

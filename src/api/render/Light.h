#pragma once

#include "../stdafx.h"

#include "Actor.h"

#include "renderer.h"
#include "shader.h"

///@todo minden cuccot egy namespace-be kellene bedobni; 
///@todo valahogy a materialba kellene belejatszani mindezt
///@todo aligned new-t mindenre
namespace Grafkit{

	class Light;
	typedef Ref<Light> LightRef; 

	__declspec(align(16))  
	class Light : virtual public Referencable, public Entity3D, public AlignedNew<Light>
	{

	public:
		enum light_type_t {
			INVALID = 0, 
			LT_point = 1 , 
			LT_directional = 2, 
			LT_spot = 3, 
			LT_ambient = 4, 
			COUNT
		};

	public:
		Light(light_type_t t = LT_point);
		~Light();

		/// @todo ez csak egy QnD Hack, ki kell majd javitani a jovoben
		float4 &Position() { return m_position; }
		void Position(float4 p) {m_position = p; }

		float4 &Direction() { return m_direction; }
		void Direction(float4 d) { m_direction = d; }

		float4 &Ambient() { return m_light.ambient; }
		float4 &Diffuse() { return m_light.diffuse; }
		float4 &Specular() { return m_light.specular; }

		float &ConstantAttenuation() { return m_light.ca; }
		float &LinearAttenuation() { return m_light.la; }
		float &QuardicAttenuation() { return m_light.qa; }

		float& Angle() { return m_light.angle; }
		float& Falloff() { return m_light.falloff; }

		/// Setup the corresponding constant buffer inside the shader
		//void SetShaderCB(ShaderRef &rPShader);

		void Render(Grafkit::Renderer& deviceContext, Scene * const & scene);
		void Calculate(Grafkit::Renderer& deviceContext, Scene * const & scene, Matrix &nodeMatrix);
		void Build(Grafkit::Renderer& deviceContext, Scene * const & scene) {}


	protected:

		enum light_type_t m_type;

		struct light_t {
			int type;
			float4 position;
			float4 direction;

			float4 ambient;
			float4 diffuse;
			float4 specular;

			float ca, la, qa;

			float angle, falloff;
		};

		struct light_t m_light;

		float4 m_position;
		float4 m_direction;

		int m_id;

	protected:
		virtual void serialize(Archive& ar);
		PERSISTENT_DECL(Grafkit::Light, 1);
	};

}

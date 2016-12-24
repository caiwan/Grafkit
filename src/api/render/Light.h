#pragma once

#include "../stdafx.h"

#include "Actor.h"

#include "renderer.h"
#include "shader.h"

///@todo minden cuccot egy namespace-be kellene bedobni; 
///@todo valahogy a materialba kellene belejatszani mindezt
///@todo aligned new-t mindenre
namespace Grafkit{

	class BaseLight;
	typedef Ref<BaseLight> LightRef; 

	class BaseLight : virtual public Referencable, public Entity3D
	{
	public:
		BaseLight();
		~BaseLight();

		/// @todo ez csak egy QnD Hack, ki kell majd javitani a jovoben
		float4 &GetPosition() { return m_light.position; }
		float4 &GetDirection() { return m_light.direction; }

		float4 &GetBasePosition() { return m_position; }
		float4 &GetBaseDirection() { return m_direction; }

		float4 &GetAmbient() { return m_light.ambient; }
		float4 &GetDiffuse() { return m_light.diffuse; }
		float4 &GetSpecular() { return m_light.specular; }

		float &GetConstantAttenuation() { return m_light.ca; }
		float &GetLinearAttenuation() { return m_light.la; }
		float &GetQuardicAttenuation() { return m_light.qa; }

		float& GetAngle() { return m_light.angle; }
		float& GetFalloff() { return m_light.falloff; }

		/// Setup the corresponding constant buffer inside the shader
		//void SetShaderCB(ShaderRef &rPShader);

		void Render(Grafkit::Renderer& deviceContext, Scene* scene);

	protected:

		///@todo ez egy kurvaszar megoldas, es nem kene hasznalni a feny tipusanak meghatarozasara; helyette virtualis fuggvennyekkel kellene~
		enum type_t {
			INVALID = 0, T_point, T_directional, T_spot, T_ambient, COUNT
		};

		virtual enum type_t GetLightType() = 0;

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

		// ez amit tarolunk a ndoeban
		float4 m_position;
		float4 m_direction;

	};

	// ============================================================================================================
	__declspec(align(16)) class PointLight : public BaseLight, public AlignedNew<PointLight>
	// class PointLight : public BaseLight 
	{
	public:
		PointLight() : BaseLight () {}
		~PointLight() {}

	private:
		virtual enum type_t GetLightType() { return T_point; }
	};

	// ============================================================================================================
	__declspec(align(16)) class DirectionalLight : public BaseLight, public AlignedNew<DirectionalLight>
	// class DirectionalLight : public BaseLight
	{
		public:
		DirectionalLight() : BaseLight() {}
		~DirectionalLight() {}

	private:
		virtual enum type_t GetLightType() { return T_directional; }

	};

	// ============================================================================================================
	__declspec(align(16)) class SpotLight : public BaseLight, public AlignedNew<SpotLight>
	// class SpotLight : public BaseLight
	{
	public:
		SpotLight() : BaseLight() {}
		~SpotLight() {}

	private:
		virtual enum type_t GetLightType() { return T_spot; }
	};

	// ============================================================================================================
	__declspec(align(16)) class AmbientLight : public BaseLight, public AlignedNew<AmbientLight>
	// class AmbientLight : public BaseLight
	{
	public:
		AmbientLight() : BaseLight() {}
		~AmbientLight() {}

	private:
		virtual enum type_t GetLightType() { return T_ambient; }
	};
}

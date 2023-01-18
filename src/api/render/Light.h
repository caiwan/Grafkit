#pragma once

#include "dxtypes.h"
#include "renderer.h"
#include "shader.h"

#include "../core/renderassets.h"

///@todo minden cuccot egy namespace-be kellene bedobni; 
///@todo valahogy a materialba kellene belejatszani mindezt
///@todo aligned new-t mindenre
namespace FWrender{

	class BaseLight;
	typedef Ref<BaseLight> LightRef; 

	class BaseLight : virtual public FWassets::IRenderAsset 
	{
	public:
		BaseLight();
		~BaseLight();

		float4 &GetPosition() { return m_light.position; }
		float4 &GetDirection() { return m_light.direction; }
			   			   
		float4 &GetAmbient() { return m_light.ambient; }
		float4 &GetDiffuse() { return m_light.diffuse; }
		float4 &GetSpecular() { return m_light.specular; }

		float &GetConstantAttenuation() { return m_light.ca; }
		float &GetLinearAttenuation() { return m_light.la; }
		float &GetQuardicAttenuation() { return m_light.qa; }

		float& GetAngle() { return m_light.angle; }
		float& GetFalloff() { return m_light.falloff; }

		/// Setup the corresponding constant buffer inside the shader
		void SetShaderCB(ShaderRef rPShader);

		virtual enum RA_type_e GetBucketID() { return IRenderAsset::RA_TYPE_Light; }

	protected:

		///@todo ez egy kurvaszar megoldas, es nem kene hasznalni a feny tipusanak meghatarozasara; helyette virtualis fuggvennyekkel kellene~
		enum type_t {
			INVALID = 0, point, directional, spot, ambient, COUNT
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

	};

	// ============================================================================================================
	
	class PointLight : public BaseLight 
	{
	public:
		PointLight() : BaseLight () {}
		~PointLight() {}

	private:
		virtual enum type_t GetLightType() { return point; }
	};

	class DirectionalLight : public BaseLight
	{
		public:
		DirectionalLight() : BaseLight() {}
		~DirectionalLight() {}

	private:
		virtual enum type_t GetLightType() { return directional; }

	};

	class SpotLight : public BaseLight
	{
	public:
		SpotLight() : BaseLight() {}
		~SpotLight() {}

	private:
		virtual enum type_t GetLightType() { return spot; }
	};

	class AmbientLight : public BaseLight
	{
	public:
		AmbientLight() : BaseLight() {}
		~AmbientLight() {}

	private:
		virtual enum type_t GetLightType() { return ambient; }
	};
}

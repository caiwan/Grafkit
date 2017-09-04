#pragma once

#include <vector>
#include <map>

#include "../utils/persistence/persistence.h"

#include "../utils/resource.h"
#include "../utils/memory.h"

#include "dxtypes.h"
#include "renderer.h"
#include "texture.h"
#include "shader.h"

namespace Grafkit {
	__declspec(align(16))
		class Material : public Grafkit::IResource, public AlignedNew<Material>, public Persistent
	{
	public:
		enum texture_type_e {
			// regular types	
			TT_diffuse,	///< 1st map

			TT_metalness,
			TT_roughness,
			TT_emission,

			TT_alpha,		///< alpha map
			TT_normal,		///< bump map
			TT_bump,		///< bump map

			TT_aux0,		///< aux texture, used for pretty much everything else
			TT_aux1,		///< aux texture, used for pretty much everything else
			TT_aux2,		///< aux texture, used for pretty much everything else
			TT_aux3,		///< aux texture, used for pretty much everything else

			TT_COUNT	// count
		};

	public:
		Material();
		~Material() {}

		/* THESE PROPERTIES ARE NOT REALLY SUPPORTED ANYMORE DUE PBR 
		I'll refactor it later on
		*/

		// TODO: QND hack for assimp converter, opt out to return w/ ref

		//float4 &GetAmbient() { return m_material.ambient; }
		float4 &GetDiffuse() { return m_colors.diffuse; }
		float4 &GetSpecular() { return m_colors.specular; }
		//float4 &GetEmission() { return m_material.emission; }

		float &GetIntensity() { return m_params.intensity; }
		float &GetHardness() { return m_params.hardness; }
		float &GetRefraction() { return m_params.refraction; }
		float &GetRoughness() { return m_params.roughness; }
		float &GetSlope() { return m_params.slope; }
		float &GetEmission() { return m_params.emission; }

		void SetIntensity(float intensity) { m_params.intensity = intensity; }
		void SetHardness(float hardness) { m_params.hardness = hardness; }
		void SetRefraction(float refraction) { m_params.refraction = refraction; }
		void SetRoughness(float roughness) { m_params.roughness = roughness; }
		void SetSlope(float slope) { m_params.slope = slope; }
		void SetEmission(float emission) { m_params.slope = emission; }

		TextureResRef GetTexture(std::string bindName);

		void SetTexture(TextureResRef texture, std::string bindName);
		void AddTexture(TextureResRef texture, std::string bindName);
		void RemoveTexture(TextureResRef texture, std::string bindName);

		void SetTexture(TextureResRef texture, texture_type_e slot);
		void AddTexture(TextureResRef texture, texture_type_e slot);
		void RemoveTexture(TextureResRef texture, texture_type_e slot);

		float GetLayer() { return m_params.layer; }
		void SetLayer(float l) { m_params.layer = l; }

		// persisthez kell
		bool _IsTextureMapEmpty() { return m_textures.empty(); }
		std::map<std::string, TextureResRef>::const_iterator _GetTextureMapIterator_Begin() { return m_textures.cbegin(); }
		std::map<std::string, TextureResRef>::const_iterator _GetTextureMapIterator_End() { return m_textures.cend(); }

		virtual void Render(Renderer& render, ShaderRef &vertexShader, ShaderRef &pixelShader);

	protected:
		struct material_color_t {
			
			// it does need a ctor 
			// to avoid compile failures for some reason
			material_color_t(){}

			float4 diffuse, specular;
		};

		struct material_params_t {

			// it does need a ctor 
			// to avoid compile failures for some reason
			material_params_t(){}

			union {
				struct {
					float intensity;	// p0.x specular level, 
					float hardness;		// p0.y shininess
					float refraction;	// p0.z
					float roughness;	// p0.w
					float slope;		// p1.x
					float emission;		// p1.y emission level of diffuse color
					float p1z, p1w;		// padding
					float p2x, p2y, p2z;	//padding
					float layer;		// layer if multiple shaders are used

				};
				float4 param0, param1, param2;
			};

		};

		struct material_color_t m_colors;
		struct material_params_t m_params;

		//unsigned int m_layer; // TODO: use multiple materials with shader in the same scene in the future 

		std::map<std::string, TextureResRef> m_textures;

	protected:
		void serialize(Archive& ar);
		PERSISTENT_DECL(Grafkit::Material, 1);

	};

	typedef Ref<Material> MaterialRef;
}

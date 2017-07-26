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

			TT_alpha,		///< alpha map
			TT_normal,		///< bump map
			TT_shiniess,	///< shininess map
			TT_specular,	///< specular map
			TT_selfillum,	///< self illumination map
			TT_reflect,		///< reflection map
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

		float4 &GetAmbient() { return m_material.ambient; }
		float4 &GetDiffuse() { return m_material.diffuse; }
		float4 &GetSpecular() { return m_material.specular; }
		float4 &GetEmission() { return m_material.emission; }

		float GetIntensity() { return m_material.intensity; }
		float GetHardness() { return m_material.hardness; }
		float GetRefraction() { return m_material.refraction; }
		float GetRoughness() { return m_material.roughness; }
		float GetSlope() { return m_material.slope; }

		void SetIntensity(float intensity) { m_material.intensity = intensity; }
		void SetHardness(float hardness) { m_material.hardness = hardness; }
		void SetRefraction(float refraction) { m_material.refraction = refraction; }
		void SetRoughness(float roughness) { m_material.roughness = roughness; }
		void SetSlope(float slope) { m_material.slope = slope; }

		TextureResRef GetTexture(std::string bindName);

		void SetTexture(TextureResRef texture, std::string bindName);
		void AddTexture(TextureResRef texture, std::string bindName);
		void RemoveTexture(TextureResRef texture, std::string bindName);

		void SetTexture(TextureResRef texture, texture_type_e slot);
		void AddTexture(TextureResRef texture, texture_type_e slot);
		void RemoveTexture(TextureResRef texture, texture_type_e slot);

		// persisthez kell
		bool _IsTextureMapEmpty() { return m_textures.empty(); }
		std::map<std::string, TextureResRef>::const_iterator _GetTextureMapIterator_Begin() { return m_textures.cbegin(); }
		std::map<std::string, TextureResRef>::const_iterator _GetTextureMapIterator_End() { return m_textures.cend(); }

		virtual void Render(Renderer& render, ShaderRef &pixelShader);

	protected:
		struct material_t {
			material_t() {}

			// 16-os alignmenttel, 4-re kell paddolni mindnet 

			union {
				struct {
					float4 ambient, diffuse, specular, emission;
				};
				char ___1[4 * 16]; // a float4 eleve 16-ra van paddolva
			};

			union {
				struct {
					float intensity;	// specular level, 
					float hardness;		// shininess
					float refraction;
					float roughness;
					float slope;
				};
				char ___2[4 * 16]; // a float4 eleve 16-ra van paddolva
			};

		};

		struct material_t m_material;

		int m_shaderLayer; // TODO: use multiple materials with shader in the same scene in the future 

		std::map<std::string, TextureResRef> m_textures;

	protected:
		void serialize(Archive& ar);
		PERSISTENT_DECL(Grafkit::Material, 1);

	};

	typedef Ref<Material> MaterialRef;
}

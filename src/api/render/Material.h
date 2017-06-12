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
		enum material_type_e {
			MT_flat,
			MT_phong_blinn,
			MT_COUNT
		};

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
		Material(enum Material::material_type_e t = MT_phong_blinn);
		~Material() {}

		/// @todo ez nem ilyen lesz a jovoben
		// operator material_t& () { return this->m_material; }

		/**
		@todo three.js szerint:
			- blending + blending mode + opacity
			- depth func + depth test -> itt mintha VS is benn lenne a shaderben, nem csak a FS
			- polygon offsr, alpha test
			- is_visible

		*/

		float4 &GetAmbient() { return m_material.ambient; }
		float4 &GetDiffuse() { return m_material.diffuse; }
		float4 &GetSpecular() { return m_material.specular; }
		float4 &GetEmission() { return m_material.emission; }
		float &GetSpecularLevel() { return m_material.specularLevel; }
		float &GetShininess() { return m_material.shininess; }

		TextureResRef GetTexture(std::string bindName);

		void SetTexture(TextureResRef texture, std::string bindName);
		void AddTexture(TextureResRef texture, std::string bindName);
		void RemoveTexture(TextureResRef texture, std::string bindName);

		void SetTexture(TextureResRef texture, texture_type_e slot);
		void AddTexture(TextureResRef texture, texture_type_e slot);
		void RemoveTexture(TextureResRef texture, texture_type_e slot);

		// persisthez kell
		std::map<std::string, TextureResRef>::const_iterator _GetTextureMapIterator_Begin() { return m_textures.cbegin(); }
		std::map<std::string, TextureResRef>::const_iterator _GetTextureMapIterator_End() { return m_textures.cend(); }

		virtual void Render(Renderer& render, ShaderRef &pixelShader);

	protected:
		struct material_t {
			material_t() {}

			union {
				int type;	/* Material tipusa */
				char ___0[16]; // az integert paddolni kell 16-ra
			};

			union {
				struct {
					float4 ambient, diffuse, specular, emission;
				};
				char ___1[4 * 16]; // a float4 eleve 16-ra van paddolva
			};

			union {
				float specularLevel;
				char ___2[4];
			};

			union {
				float shininess;
				char ___3[4];
			};

			// miserable hack
			// has to have this thing for a while for padding in serializer
			union {
				int has_texture[TT_COUNT];
				// ezt mire kell paddolni
				char ___4[TT_COUNT * 16];
			};

		};

		struct material_t m_material;

		std::map<std::string, TextureResRef> m_textures;

		// ShaderRef m_override_fshader;


	protected:
		void serialize(Archive& ar);
		PERSISTENT_DECL(Grafkit::Material, 1);

	};

	typedef Ref<Material> MaterialRef;
}

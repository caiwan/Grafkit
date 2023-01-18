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

		struct material_color_t {

			// it does need a ctor 
			// to avoid compile failures for some reason
			material_color_t() {}

			float4 diffuse, specular, emission;
		};

		struct material_params_t {

			// it does need a ctor 
			// to avoid compile failures for some reason
			material_params_t() {}

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

		enum texture_type_e {
			// regular types	
			TT_diffuse,		///< 1st map diffuse = albedo

			TT_metallic,
			TT_roughness,	
			TT_emission,

			TT_alpha,		///< alpha map
			TT_normal,		///< normal map
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

		// qnd thingy
		material_color_t& Colors() { return m_colors; }
		material_params_t& Params() { return m_params; }

		TextureResRef GetTexture(std::string bindName);

		void SetTexture(TextureResRef texture, std::string bindName);
		void AddTexture(TextureResRef texture, std::string bindName);
		void RemoveTexture(TextureResRef texture, std::string bindName);

		void SetTexture(TextureResRef texture, texture_type_e slot);
		void AddTexture(TextureResRef texture, texture_type_e slot);
		void RemoveTexture(TextureResRef texture, texture_type_e slot);

		virtual void Render(Renderer& render, ShaderRef &vertexShader, ShaderRef &pixelShader);

		// --- 
		// persisthez kell
		bool GetTextureMap(std::map<std::string, TextureResRef> &textureMap);

	protected:
		struct material_color_t m_colors;
		struct material_params_t m_params;

		std::map<std::string, TextureResRef> m_textures;

	protected:
		void serialize(Archive& ar);
		PERSISTENT_DECL(Grafkit::Material, 1);
	};

	typedef Ref<Material> MaterialRef;
}

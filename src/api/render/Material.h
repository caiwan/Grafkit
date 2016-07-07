#pragma once

#include <vector>
#include <map>

#include "../utils/resource.h"
#include "../utils/memory_align.h"

#include "dxtypes.h"
#include "renderer.h"
#include "texture.h"
#include "shader.h"

namespace Grafkit {

	///@todo three.js + hieroglyph3 mintajara tobbfele materrial tipust lehessen legyartani, ha kell~
	///@todo aligned new-t mindenre
	__declspec(align(16)) class BaseMaterial : public Grafkit::IResource , public AlignedNew<BaseMaterial>
	//class BaseMaterial : public Grafkit::IResource 
	{
	public:
		BaseMaterial() : IResource(), m_material() {};
		~BaseMaterial() {}

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

		virtual void Render(Renderer& render, ShaderRef &shader);

	protected:
		struct material_t {
			int type;	/* Material tipusa */
			float4 ambient, diffuse, specular, emission;
			float specularLevel;
			float shininess;
			
		};

		struct material_t m_material;

		std::map<std::string, TextureResRef> m_textures;

	};

	typedef Ref<BaseMaterial> MaterialRef;

	/// @Todo material generator + kulonbozo tipusu materialok generalasa

	/* 
	+ Flat
	+ Phong
	+ Phong-Blinn
	+ Lambert
	+ Shader
	*/

}

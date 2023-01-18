#pragma once

#include <vector>
#include <map>

#include "dxtypes.h"
#include "renderer.h"
#include "texture.h"
#include "shader.h"

#include "../core/resource.h"

#include "memory_align.h"

namespace Grafkit {

	/// Using materials as a stuct. For now. 
	struct Material_t {
		float4 T_ambient, diffuse, specular, emission;
		float specularLevel;
		float shininess;
	};


#define MATERIAL_BUCKET "material"

	///@todo three.js + hieroglyph3 mintajara tobbfele materrial tipust lehessen legyartani, ha kell~
	///@todo aligned new-t mindenre
	__declspec(align(16)) class MaterialBase : public Grafkit::IResource , public AlignedNew<MaterialBase>
	//class MaterialBase : public Grafkit::IResource 
	{
	public:
		MaterialBase() : IResource(), m_material() {};
		~MaterialBase() {}

		/// @todo ez nem ilyen lesz a jovoben
		operator Material_t& () { return this->m_material; }

		/**
		@todo three.js szerint:
			- blending + blending mode + opacity
			- depth func + depth test -> itt mintha VS is benn lenne a shaderben, nem csak a FS
			- polygon offsr, alpha test
			- is_visible

		@todo consider:
			- VS hosszadasa 
			- shadernek valamilyen property managert kell szulni meg, amit ez kezelni fog
		*/

		/// @todo valahogy ide valamilyen kulcs-ertek parok formajaban kellene a cbuffer cuccait betolni. 
		float4 &GetAmbient() { return m_material.T_ambient; }
		float4 &GetDiffuse() { return m_material.diffuse; }
		float4 &GetSpecular() { return m_material.specular; }
		float4 &GetEmission() { return m_material.emission; }
		float &GetSpecularLevel() { return m_material.specularLevel; }
		float &GetShininess() { return m_material.shininess; }

		TextureResRef GetTexture(std::string bindName /*texture_type_e bucket, int n = 0*/);

		void SetTexture(TextureResRef texture, std::string bindName);
		void AddTexture(TextureResRef texture, std::string bindName);
		void RemoveTexture(TextureResRef texture, std::string bindName);

		virtual void Render(Renderer& render, ShaderRef &shader);

		virtual const char* GetBucketID() { return MATERIAL_BUCKET; }

	protected:
		struct Material_t m_material;

		typedef std::map<std::string, TextureResRef> textureMap_t;
		typedef textureMap_t::iterator textureMap_it_t;

		textureMap_t m_textures;

	};

	typedef Ref<MaterialBase> MaterialRef;

	/// @Todo material generator + kulonbozo tipusu materialok generalasa

	/// @todo 

}


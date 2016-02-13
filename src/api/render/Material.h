#pragma once

#include <vector>

#include "dxtypes.h"
#include "reference.h"
#include "renderer.h"
#include "shader.h"

#include "../core/renderassets.h"

namespace FWrender {

	/// Using materials as a stuct. For now. 
	struct Material {
		float4 ambient, diffuse, specular, emission;
		float specularLevel;
		float shininess;
	};

	///Based on 3ds format's 
	enum texture_type_e {
		// regular types	
		TT_diffuse,	///< 1st map
		
		TT_alpha,		///< alpha map
		TT_normal,		///< bump map
		TT_shiniess,   ///< shininess map
		TT_specular,   ///< specular map
		TT_selfillum,	///< self illumination map
		TT_reflect,	///< reflection map
		TT_bump,		///< bump map

		// ezek valoszinuleg post-effektezeshez kellenenek, ami nem itt lesz szamontartva lehetoleg
		/*
		TMT_shadowDepthtMap, ///< for depth map shadow
		TMT_sceneDephtMap,	///< depth map of the scene
		TMT_sceneTexture,	///< the scne rendered into a texture
		TMT_glowFilter,	    ///< input of the glow filter
		*/

		TT_aux, ///< aux texture, used for pretty much everything else

		TT_COUNT	// count
	};

	///@todo three.js mintajara tobbfele materrial tipust lehessen legyartani, ha kell~ 
	class MaterialBase : virtual public Referencable, virtual public FWassets::IRenderAsset
	{
	public:
		MaterialBase();
		~MaterialBase();

		/// @todo ez nem ilyen lesz a jovoben
		operator Material& () { return this->m_mater; }

		/// @todo valahogy ide valamilyen kulcs-ertek parok formajaban kellene a cbuffer cuccait betolni. 
		float4 &GetAmbient() { return m_mater.ambient; }
		float4 &GetDiffuse() { return m_mater.diffuse; }
		float4 &GetSpecular() { return m_mater.specular; }
		float4 &GetEmission() { return m_mater.emission; }
		float &GetSpecularLevel() { return m_mater.specularLevel; }
		float &GetShininess() { return m_mater.shininess; }

		ShaderRef GetShader() { return this->m_framgentShader; }
		void SetShader(ShaderRef shader) { this->m_framgentShader = shader; ReflectShader(); }

		/// @todo slotok kezelese 
		/// @todo bounds check
		TextureRef &GetTexture(texture_type_e bucket, int n = 0);
		void SetTexture(TextureRef texture, int n = 0);

		size_t GetTextureBucketCount(texture_type_e bucket) { return this->m_texture_buckets[bucket].size(); }

		virtual void Render(ID3D11DeviceContext* deviceContext);

	protected:
		/// Setup texture and cbufer locations
		virtual void ReflectShader();
		void ReflectTextures();

		struct Material m_mater;

		std::vector<TextureRef> m_texture_buckets[TT_COUNT];
		ShaderRef m_framgentShader;

		struct reflection_texture_entity 
		{
			FWrender::Shader::BoundResourceRecord *brecord;
			TextureRef texture;
		};

		std::vector<struct reflection_texture_entity> m_reflected_textures;
	};

	typedef Ref<MaterialBase> MaterialRef;

}

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

		///@todo ide be kellene meg jatszani a textura szurot meg, jol.

		/// @todo slotok kezelese -> a texturaknak legyen sajat nevuk is
		/// @todo bounds check
		TextureRef &GetTexture(texture_type_e bucket, int n = 0);
		void SetTexture(TextureRef texture, texture_type_e bucket = TT_diffuse, int n = 0);
		void AddTexture(TextureRef texture, texture_type_e bucket = TT_diffuse);

		size_t GetTextureBucketCount(texture_type_e bucket) { return this->m_texture_buckets[bucket].size(); }

		virtual void Render(ID3D11DeviceContext* deviceContext);

		/// Setup texture and cbufer locations
		/// Legyen public arra az esetre, ha megis ... 
		virtual void ReflectShader();

	protected:
		/**
			Will look for textures with name `texture_%type%%count%`
			- where `%type%` is textur type in string
			- where `%count%` is bucket count if multiple
		*/
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

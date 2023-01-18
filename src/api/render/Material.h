#pragma once

#include "dxtypes.h"
#include "reference.h"
#include "../core/renderassets.h"

namespace FWrender {

	/// Using materials as a stuct. For now. 
	struct Material {
		float4 ambient, diffuse, specular, emission;
		float specularLevel;
		float shininess;
	};

	class MaterialWrapper : virtual public Referencable, virtual public FWassets::IRenderAsset
	{
	public:
		MaterialWrapper() {}
		~MaterialWrapper() {}

		operator Material& () { return this->m_mater; }

		float4 &GetAmbient() { return m_mater.ambient; }
		float4 &GetDiffuse() { return m_mater.diffuse; }
		float4 &GetSpecular() { return m_mater.specular; }
		float4 &GetEmission() { return m_mater.emission; }
		float &GetSpecularLevel() { return m_mater.specularLevel; }
		float &GetShininess() { return m_mater.shininess; }

	protected:
		struct Material m_mater;
	};

	typedef Ref<MaterialWrapper> MaterialRef;

}

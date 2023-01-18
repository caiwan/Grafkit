#pragma once

#include "reference.h"

namespace Grafkit {

	class IAsset;
	typedef Ref<IAsset> IAssetRef;

	/// an interface for resources
	class IAsset : public Referencable
	{
	public:
		IAsset() {}
		virtual ~IAsset() {}

		virtual void* GetData() const = 0;
		virtual size_t GetSize() const = 0;
	};

	// ----------

}

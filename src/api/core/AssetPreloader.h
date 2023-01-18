#pragma once
#include "renderassets.h"

namespace FWassets{
	/**
	Ez egy olyan dolog, ami automatikusan betolt, es cachel objektumokat
	*/
	class AssetPreloader : public FWassets::IRenderAssetManager
	{
	public:
		AssetPreloader();
		~AssetPreloader();
	};
}

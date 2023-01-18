/** 
	Resource builder interface
*/
#pragma once 

#include "Asset.h"
#include "Resource.h"

namespace Grafkit {

	class IResourceManager;

	/**
	Ezt gyujti be a preloader, es gyartja le a megfelelo asseteket
	src -> dst
	*/
	class IResourceBuilder
	{
		public:
			IResourceBuilder(std::string name, IResourceRef &dst) : m_srcName(name), m_dstResource(dst) {}
			virtual ~IResourceBuilder() {}
			
			void operator () (IResourceManager * const & assman);

			virtual void load(IResourceManager * const & assman) = 0;

	protected:
		IAssetRef GetSourceAsset(IResourceManager * const & assman);
		std::string m_srcName;
		IResourceRef &m_dstResource;
	};
	
}


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
			IResourceBuilder(std::string name, std::string sourcename, IResourceRef &dst) : m_name(name), m_srcName(name), m_dstResource(dst) {}
			virtual ~IResourceBuilder() {}
			
			///@todo operator()-t eliminalni kell majd 
			void operator () (IResourceManager * const & assman);

			virtual void load(IResourceManager * const & assman) = 0;

	protected:
		IAssetRef GetSourceAsset(IResourceManager * const & assman);
		std::string m_name;
		std::string m_srcName;
		IResourceRef &m_dstResource;
	};
	
}


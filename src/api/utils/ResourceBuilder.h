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
			IResourceBuilder(std::string name, std::string sourcename) : m_name(name), m_srcName(sourcename) {}
			virtual ~IResourceBuilder() {}

			std::string GetName() { return m_name; }

			// pure virtuals 
			virtual IResource* NewResource() = 0;

			virtual void Load(IResourceManager * const & resman, IResource * source) = 0;

	protected:
		IAssetRef GetSourceAsset(IResourceManager * const & assman);

		std::string m_name;
		std::string m_srcName;
	};

}

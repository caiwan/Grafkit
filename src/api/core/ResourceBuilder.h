/** 
	Resource builder interface
*/
#pragma once 

namespace Grafkit {

	class IResourceManager;

	/**
	Ezt gyujti be a preloader, es gyartja le a megfelelo asseteket
	*/
	class IResourceBuilder
	{
		public:
			IResourceBuilder() {}
			virtual ~IResourceBuilder() {}
			
			virtual void operator () (IResourceManager * const & assman) = 0;
	};
	
}


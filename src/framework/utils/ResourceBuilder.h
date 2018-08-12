/**
    Resource builder interface
*/
#pragma once 

#include "Asset.h"
#include "Resource.h"

namespace Grafkit {

    class IResourceManager;

    class IResourceBuilder
    {
    public:
        //explicit IResourceBuilder(std::string name) : m_name(name), m_srcName(), m_uuid() {}
        explicit IResourceBuilder(std::string name, std::string sourcename = "", std::string uuid = "") : m_name(name), m_srcName(sourcename), m_uuid(uuid) {}

        virtual ~IResourceBuilder() {}

        std::string GetName() const { return m_name; }
        std::string GetUuid() const { return m_uuid; }
        std::string GetSourceName() const { return m_srcName; }

        // pure virtuals 
        virtual IResource* NewResource() = 0;

        virtual void Load(IResourceManager * const & resman, IResource * source) = 0;

    protected:
        IAssetRef GetSourceAsset(IResourceManager * const & assman) const;

        std::string m_name;
        std::string m_srcName;
        std::string m_uuid;
    };

}

/**
    Resource builder interface
*/
#pragma once 

#include "Asset.h"
#include "Resource.h"

namespace Grafkit {

    class IResourceManager;

    class IResourceBuilder : public Object
    {
    public:
        explicit IResourceBuilder(){}
        explicit IResourceBuilder(std::string name, std::string sourcename = "", std::string uuid = "") : Object(name, uuid), /*m_name(name), m_uuid(uuid),*/ m_srcName(sourcename) {}

        virtual ~IResourceBuilder() {}

        //std::string GetName() const { return m_name; }
        //std::string GetUuid() const { return m_uuid; }
        std::string GetSourceName() const { return m_srcName; }

        // pure virtuals 
        virtual IResource* NewResource() = 0;

        virtual void Load(IResourceManager * const & resman, IResource * source) = 0;

    protected:
        IAssetRef GetSourceAsset(IResourceManager * const & assman) const;

        //std::string m_name;
        std::string m_srcName;
        //std::string m_uuid;

    public:
        Clonable * CreateObj() const override { assert(0); return nullptr; }
    protected:
        void Serialize(Archive& ar) override { assert(0); }
        std::string GetClazzName() const override { assert(0); return std::string(); }
    };

}

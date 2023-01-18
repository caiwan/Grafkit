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
        explicit IResourceBuilder(std::string name, std::string sourcename = "", std::string uuid = "") : Object(name, uuid), /*m_name(name), m_uuid(uuid),*/ m_sourceName(sourcename) {}

        virtual ~IResourceBuilder() {}

        std::string GetSourceName() const { return m_sourceName; }

        // pure virtuals 
        virtual IResource* NewResource() = 0;

        virtual void Load(IResourceManager * const & resman, IResource * source) = 0;

    protected:
        IAssetRef GetSourceAsset(IResourceManager * const & assman) const;

        std::string m_sourceName;

    public:
        Clonable * CreateObj() const override { assert(0); return nullptr; }
    protected:
        void Serialize(Archive& ar) override { assert(0); }
        std::string GetClazzName() const override { assert(0); return std::string(); }
    };

}

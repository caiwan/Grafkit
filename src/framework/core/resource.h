/**
A generator interface for assets
*/

#pragma once

#include "core/Object.h"
#include "reference.h"

namespace Grafkit
{
    class IResource;
    class IResourceManager;
    class IResourceBuilder;

    /**
    A base class for collectable render assets for bulk loading and reloading
    */
    class IResource : public Object
    {
        friend class IResourceManager;
    public:
        IResource(){};

        explicit IResource(const std::string& name) : Object(name){}
        explicit IResource(const std::string& name, const std::string& uuid) : Object(name, Uuid(uuid)) {}

        virtual ~IResource() = default;

        virtual const void * GetRaw() const = 0;
    };

    /**
     *TOOD: Get rid of Ref<T>
    */
    template <typename T>
    class Resource : public IResource, public Ref<T>
    {
    public:
        Resource() : IResource()
            , Ref<T>() {
        }

        explicit Resource(Resource* ptr) : IResource(ptr)
            , Ref<T>(ptr) {
        }

        explicit Resource(Ref<Resource> ref) : IResource(ref)
            , Ref<T>(ref) {
        }

        explicit Resource(Ref<T> tref) : IResource()
            , Ref<T>(tref) {
        }

        explicit Resource(T* tptr) : IResource()
            , Ref<T>(tptr) {
        }

        explicit Resource(Ref<T> tref, const std::string& name) : IResource(name)
            , Ref<T>(tref) {
        }

        explicit Resource(Ref<T> tref, const std::string& name, const std::string& uuid) : IResource(name, uuid)
            , Ref<T>(tref) {
        }

        explicit operator Ref<T>() { return Ref<T>(dynamic_cast<T*>(this->Get())); }
        explicit operator T * const &() { return dynamic_cast<T*>(this->Get()); }
        explicit operator T&() { return *this->Get(); }

        const void * GetRaw() const override { return ptr; }

    // ???
    protected:
        std::string GetClazzName() const override { return {}; }
        uint16_t GetVersion() const override { return 0; }
    };
}

/**
A generator interface for assets
*/

#pragma once

#include <string>

#include "core/Object.h"

#include "reference.h"

namespace Grafkit {

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
        IResource();

        IResource(const IResource& other);

        IResource& operator=(const IResource& other);

        explicit IResource(const std::string& name);
        explicit IResource(const std::string& name, const std::string& uuid);

        virtual ~IResource();

        virtual void* GetRaw() = 0;

    };

    /**
    Ebbe csomagoljuk bele a resourceokat, amiket elteszunk; lenyeg, hogy ezzel barmibol
    lehet majd resouce-t generalni, ha kell
    */
    template <typename T> class Resource : public IResource, public Ref<T>
    {
    public:
        Resource() : IResource(), Ref<T>() {}

        explicit Resource(Resource* ptr) : IResource(ptr), Ref<T>(ptr) {}
        explicit Resource(Ref<Resource> ref) : IResource(ref), Ref<T>(ref) {}

        explicit Resource(Ref<T> tref) : IResource(), Ref<T>(tref) {}
        explicit Resource(T* tptr) : IResource(), Ref<T>(tptr) {}

        explicit Resource(Ref<T> tref, const std::string& name) : IResource(name), Ref<T>(tref) {}
        //explicit Resource(T* tptr, const std::string& name) : IResource(name), Ref<T>(tptr) {}

        explicit Resource(Ref<T> tref, const std::string& name, const std::string& uuid) : IResource(name, uuid), Ref<T>(tref) {}
        //explicit Resource(T* tptr, const std::string& name, const std::string& uuid) : IResource(name, uuid), Ref<T>(tptr) {}

        explicit operator Ref<T>() { return Ref<T>(dynamic_cast<T*>(this->Get())); }
        explicit operator T * const & () { return dynamic_cast<T*>(this->Get()); }
        explicit operator T& () { return *(this->Get()); }

        void* GetRaw() override { return this->Get(); }

    protected:
        void Serialize(Archive& ar) override;
        PERSISTENT_DECL(Grafkit::Resource<T>, 1)
    };

    template <typename T>
    void Resource<T>::Serialize(Archive& ar) {
        _Serialize(ar);
    }
}

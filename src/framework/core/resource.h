/**
A generator interface for assets
*/

#pragma once

#include <string>
#include <typeinfo>

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
        IResource();

        IResource(const IResource& other);

        IResource& operator=(const IResource& other);

        explicit IResource(const std::string& name);
        explicit IResource(const std::string& name, const std::string& uuid);

        virtual ~IResource();

        virtual void* GetRaw() const = 0;
   
    protected:
        PERSISTENT_DECL(Grafkit::IResource, 1)
        void Serialize(Archive& ar) override ;
    };

    /**
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

        void* GetRaw() const override { return this->Get(); }
    };

    //template <typename T>
    //void Resource<T>::Serialize(Archive& ar) { _Serialize(ar); }

    //class ObjectResource : public Resource<Object>
    //{
    //public:

    //    ObjectResource() {
    //    }

    //    explicit ObjectResource(Resource* ptr)
    //        : Resource<Object>(ptr) {
    //        SetName(ptr->GetName());
    //        SetUuid(ptr->GetUuid());
    //    }

    //    explicit ObjectResource(const Ref<Resource>& ref)
    //        : Resource<Object>(ref) {
    //        SetName(ref->GetName());
    //        SetUuid(ref->GetUuid());
    //    }

    //    explicit ObjectResource(const Ref<Object>& tref)
    //        : Resource<Object>(tref) {
    //        SetName(tref->GetName());
    //        SetUuid(tref->GetUuid());
    //    }

    //    explicit ObjectResource(Object* tptr)
    //        : Resource<Object>(tptr) {
    //        SetName(tptr->GetName());
    //        SetUuid(tptr->GetUuid());
    //    }

    //   void AssingnRef(Object *p)
    //    {
    //       SetName(p->GetName());
    //       SetUuid(p->GetUuid());
    //       Resource<Object>::AssingnRef(p);
    //    }

    //    ~ObjectResource() override {};
    //    void* GetRaw() const override {};

    //};
}

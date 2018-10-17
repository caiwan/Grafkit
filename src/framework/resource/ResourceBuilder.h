#pragma once

#include "core/Object.h"

namespace Grafkit
{
    class Clonable;
    class IResource;
    class IResourceManager;

    class IResourceBuilder : public Object
    {
    public:
        IResourceBuilder() {
        }

        explicit IResourceBuilder(std::string name, Uuid uuid = Uuid()) : Object(name, uuid) {
        }

        virtual ~IResourceBuilder() {
        }

        // pure virtuals 
        virtual Ref<IResource> NewResource() const = 0;

        virtual void Load(IResourceManager* const & resman, IResource* source) = 0;
        virtual void Initialize(Renderer& render, IResourceManager* const & resman, IResource* source) = 0;

        /* It needs something that pulls serialiezed stuff as well*/

        virtual std::string GetWatcherFilename() { return std::string(); };

        // --- 
        //virtual const void* GetParamP() const = 0;
        //virtual void SetParamP(const void*) = 0;

        template <class A> void Serialize(A &ar) { Object::Serialize(ar); }
    };

    struct None
    {
        template<class A > void Serialize(A & ar){}
    };

    template <class R, typename U = None>
    class ResourceBuilder : public IResourceBuilder
    {
    public:

        ResourceBuilder() : m_params(){
        }

        explicit ResourceBuilder(const U &params) : IResourceBuilder()
            , m_params(params) {
        }

        explicit ResourceBuilder(std::string name, std::string uuid, const U &params = U()) : IResourceBuilder(name, Uuid(uuid))
            , m_params(params) {
        }

        ~ResourceBuilder() = default;

        U GetParams() const { return m_params; }
        void SetParams(const U& params) { m_params = params; }

        // fuck this 
        //// if you're brave enoguh
        //// this should be protected 
        //const void* GetParamP() const override { return reinterpret_cast<const void*>(&m_params); }
        //void SetParamP(const void* p) override { assert(p); m_params = *(reinterpret_cast<const U*>(p)); }

        Ref<IResource> NewResource() const override { return new Resource<R>(); }

        template <class A> void Serialize(A &ar) { IResourceBuilder::Serialize(ar); ar&m_params; }

    protected:
        static Ref<Resource<R>> SafeGetResource(IResource* source)
        {
            Resource<R>* pR = dynamic_cast<Resource<R>*>(source);
            assert(pR);
            return pR;
        }

        static Ref<R> SafeGetObject(IResource* source)
        {
            Resource<R>* pR = SafeGetResource(source);
            R* obj = pR->Get();
            assert(obj);
            return obj;
        }

        U m_params;
        Ref<R> m_oldResource;
    };
}

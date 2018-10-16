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
        explicit IResourceBuilder() : Object() {
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

    };

    struct None{};

    template <class R, typename U = None>
    class ResourceBuilder : public IResourceBuilder
    {
    public:

        explicit ResourceBuilder(const U params = U()) : IResourceBuilder()
            , m_params(params) {
        }

        explicit ResourceBuilder(std::string name, std::string uuid, const U params = U()) : IResourceBuilder(name, Uuid(uuid))
            , m_params(params) {
        }

        ~ResourceBuilder() = default;

        U GetParams() const { return m_params; }
        void SetParams(const U& params) { m_params = params; }

        /* It needs something that pulls parameters from void* */

        Ref<IResource> NewResource() const override { return new Resource<R>(); }

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

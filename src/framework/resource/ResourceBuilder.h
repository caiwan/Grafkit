#pragma once


namespace Grafkit
{
    class IAssetFactory;
    typedef Ref<IResource> IResourceRef;

    class IResourceLoader
    {
        friend class ResourceManager;
    public:
        virtual ~IResourceLoader() = default;

        IResourceLoader() {
        }

        explicit IResourceLoader(const Uuid& id)
            : m_id(id) {
        }

        Uuid GetId() const { return m_id; }
        void SetId(const Uuid& id) { m_id = id; }

    protected:
        virtual std::shared_ptr<IResource> CreateResource() = 0;
        virtual void Load(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory& assetFactory ) = 0;
        virtual void Initialize(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory& assetFactory) = 0;

        Uuid m_id;
    };


    struct None
    {
        template<class A > void Serialize(A & ar) {}
    };

# if 0

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
        virtual IResourceRef NewResource() const = 0;

        virtual void Load(IResourceManager* const & resman, IResource* source) = 0;
        virtual void Initialize(Renderer& render, IResourceManager* const & resman, IResource* source) = 0;

        /* It needs something that pulls serialiezed stuff as well*/

        virtual std::string GetWatcherFilename() { return std::string(); };

        // --- 
        //virtual const void* GetParamP() const = 0;
        //virtual void SetParamP(const void*) = 0;

        template <class A> void Serialize(A &ar) { Object::Serialize(ar); }
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

        IResourceRef NewResource() const override { return new Resource<R>(); }

        template <class A> void Serialize(A &ar) { IResourceBuilder::Serialize(ar); ar&m_params; }

    protected:
        static Ref<Resource<R>> SafeGetResource(IResource* source)
        {
            auto pR = dynamic_cast<Resource<R>*>(source);
            assert(pR);
            return pR;
        }

        static Ref<R> SafeGetObject(IResource* source)
        {
            auto pR = SafeGetResource(source);
            R* obj = pR->Get();
            assert(obj);
            return obj;
        }

        U m_params;
        Ref<R> m_oldResource;
    };

#endif 

    template <class T, typename P = None>
    class ResourceLoader : public IResourceLoader
    {
    public:
        typedef T myResource_t;
        typedef std::shared_ptr<T> myResourceRef_t;
        typedef P myParameter_t;

        explicit ResourceLoader(const Uuid& id, const P& params = {}) : IResourceLoader(id)
            , m_params(params) {
        }

        P GetParams() const { return m_params; }
        void SetParams(const P& params) { m_params = params; }

    protected:
        std::shared_ptr<IResource> CreateResource() override { return std::make_shared<ResourceWrapper<T>>(m_id, nullptr); }

        template <class R = myResource_t>
        Resource<R>
            static CastResource(const std::shared_ptr<IResource>& resource)
        {
            auto ptr = std::dynamic_pointer_cast<ResourceWrapper<T>>(resource);
            assert(ptr);
            return Resource<R>(ptr);
        }

        P m_params;
    };


}

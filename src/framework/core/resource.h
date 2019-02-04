/**
A generator interface for assets
*/

#pragma once

#include <typeindex>

namespace Grafkit
{
    class Uuid
    {
    public:
        Uuid() : m_uuid(Create()) {
        }

        explicit Uuid(const std::string& uuid)
            : m_uuid(uuid) {
        }

        explicit Uuid(const char* uuid)
            : m_uuid(uuid) {
        }

        Uuid(const Uuid& other)
            : m_uuid(other.m_uuid) {
        }

        Uuid& operator=(const Uuid& other)
        {
            if (this == &other)
                return *this;
            m_uuid = other.m_uuid;
            return *this;
        }

        bool IsEmpty() const { return m_uuid.empty(); }

        explicit operator std::string() const { return m_uuid; }

        const char* c_str() const { return m_uuid.c_str(); }

        friend bool operator==(const Uuid& lhs, const Uuid& rhs) { return lhs.m_uuid == rhs.m_uuid; }
        friend bool operator!=(const Uuid& lhs, const Uuid& rhs) { return !(lhs == rhs); }

        friend bool operator<(const Uuid& lhs, const Uuid& rhs) { return lhs.m_uuid < rhs.m_uuid; }

        template <class Archive>
        void Serialize(Archive& ar) { ar & m_uuid; }

    private:
        static std::string Create();
        std::string m_uuid;
    };

    class IResource;
    class ResourceManager;

    template <class T>
    class Resource;

    template <class T>
    class ResourceWrapper;

    class IResource
    {
        friend class ResourceManager;

        template <class T>
        friend class Resource;

        template <class T>
        friend class ResourceWrapper;

    public:

        IResource() {
        }

        explicit IResource(const Uuid& id)
            : m_id(id) {
        }

        virtual ~IResource() {
        }

        Uuid GetId() const { return m_id; }

        virtual std::type_index GetTypeIndex() const = 0;

    protected:
        void SetId(const Uuid& id) { m_id = id; }
        Uuid m_id;
    };

    template <class T>
    class ResourceWrapper : public IResource
    {
    public:
        template <class R>
        friend class ResourceWrapper;

        typedef T value_t;
        typedef std::shared_ptr<T> valuePtr_t;

        ResourceWrapper() : m_ptr(nullptr) {
        }

        ResourceWrapper(const ResourceWrapper& other) = delete;

        ResourceWrapper& operator=(const ResourceWrapper& other) = delete;

        explicit ResourceWrapper(std::shared_ptr<T> ptr)
            : m_ptr(ptr) {
        }

        explicit ResourceWrapper(const Uuid& id, std::shared_ptr<T> ptr) : IResource(id)
            , m_ptr(ptr) {
        }

        virtual ~ResourceWrapper() {
        }

        std::shared_ptr<T> Get() const { return m_ptr; }
        void Set(const std::shared_ptr<T>& ptr) { m_ptr = ptr; }

        std::type_index GetTypeIndex() const override { return std::type_index(typeid(*m_ptr.get())); }

    private:
        std::shared_ptr<T> m_ptr;
    };

    template <class T>
    class Resource
    {
        template <class U>
        friend class Resource;
    public:
        Resource() = default;
        Resource(const Resource<T>& other) = default;
        Resource& operator=(const Resource<T>& other) = default;

        template <class U>
        explicit Resource(const std::shared_ptr<U>& ptr) { AssignRef(ptr); }

        // Arbitrary replace the underlying resource; a.k.a. replaces all the instances 
        template <class U>
        void AssignRef(const std::shared_ptr<U>& ptr)
        {
            // Convert ResourceWrapper<R> -> ResourceWrapper<T>
            if constexpr
                (std::is_base_of<IResource, U>().value)
            {
                static_assert(std::is_base_of<T, typename U::value_t>() || std::is_base_of<typename U::value_t, T>());
                m_wrapperPtr = std::reinterpret_pointer_cast<ResourceWrapper<T>>(ptr);
            }
            // Convert R -> T then wrap it 
            else
            {
                static_assert(std::is_base_of<T, U>() || std::is_base_of<U, T>());
                if (!ptr)
                {
                    m_wrapperPtr->Set(nullptr);
                    return;
                }
                auto myPtr = std::dynamic_pointer_cast<T>(ptr);
                assert(myPtr);
                if (!m_wrapperPtr) { m_wrapperPtr = std::make_shared<ResourceWrapper<T>>(myPtr); }
                else { m_wrapperPtr->Set(myPtr); }
            }
        }

        std::shared_ptr<T> operator->() const
        {
            assert(m_wrapperPtr->Get());
            return m_wrapperPtr->Get();
        }

        std::shared_ptr<T> operator*() const
        {
            assert(m_wrapperPtr->Get());
            return m_wrapperPtr->Get();
        }

        template <class U>
        explicit operator Resource<U>() const { return CastTo<U>(); }

        template <class U>
        Resource<U> CastTo() const
        {
            static_assert(std::is_base_of<T, U>() || std::is_base_of<U, T>());
            if (Invalid())
                return Resource<U>();
            auto myPtr = std::reinterpret_pointer_cast<ResourceWrapper<T>>(m_wrapperPtr);
            return Resource<U>(myPtr);
        }

        bool Valid() const { return m_wrapperPtr && m_wrapperPtr->Get(); }
        bool Invalid() const { return !Valid(); }
        explicit operator bool() const { return Valid(); }

        bool Empty() const { return !m_wrapperPtr; }

        const Uuid GetId() const { return  m_wrapperPtr->GetId(); }

    protected:

        std::shared_ptr<ResourceWrapper<T>> m_wrapperPtr;
    };
}

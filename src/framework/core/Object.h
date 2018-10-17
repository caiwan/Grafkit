#pragma once

#include"core/reference.h"

#include "utils/persistence/persistence.h"

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

        operator std::string() const { return m_uuid; }

        const char * c_str() const { return m_uuid.c_str(); }

        friend bool operator==(const Uuid& lhs, const Uuid& rhs) { return lhs.m_uuid == rhs.m_uuid; }
        friend bool operator!=(const Uuid& lhs, const Uuid& rhs) { return !(lhs == rhs); }

        template <class Archive>
        void Serialize(Archive& ar) { ar & m_uuid; }

    private:
        static std::string Create();
        std::string m_uuid;
    };

#define OBJECT_RELEASE() m_allocation--

    class Object : public Serializable, public Referencable
    {
    public:

        Object() : m_allocation(-1)
        {
        }

        explicit Object(const std::string& name, const Uuid& uuid = Uuid())
            : m_name(name)
            , m_uuid(uuid)
            , m_allocation(-1) {
        }

        Object(const Object& other) = delete;
        Object& operator=(const Object& other) = delete;

        virtual ~Object()
        {
            // fire it up when ready to ... 
#if 0
            assert(m_allocation == 0); // ... shot ourselves in the foot 
#endif
    }

        std::string GetName() const { return m_name; }
        void SetName(const std::string& name) { m_name = name; }

        Uuid GetUuid() const { return m_uuid; }
        void SetUuid(const Uuid& uuid) { m_uuid = uuid; }

        virtual void Shutdown() //= 0;
        {
            OBJECT_RELEASE();
        }

    protected:
        std::string m_name;
        Uuid m_uuid;

        int m_allocation;

    public:
        template <class Archive>
        void Serialize(Archive& ar)
        {
            ar & m_name;
            ar & m_uuid;
        }
};

}

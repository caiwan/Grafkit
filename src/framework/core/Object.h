#pragma once

#include"utils/reference.h"

#include "utils/persistence/persistence.h"

namespace Grafkit
{
    class Uuid;

    class Object : public Persistent, virtual public Referencable
    {
    public:
        Object();
        explicit Object(const std::string& name, const std::string& uuid);
        explicit Object(const std::string& name);

        virtual ~Object();

        Object(const Object& other);

        Object& operator=(const Object& other) = delete;

        std::string GetName() const { return m_name; }
        void SetName(const std::string& name) { m_name = name; }

        std::string GetUuid() const { return m_uuid; }
        void SetUuid(const std::string& uuid) { m_uuid = uuid; }
        void CreateUuid();

    protected:
        std::string m_name;
        std::string m_uuid;

        void _Serialize(Archive& ar);
    };


    class Uuid
    {
        // TODO ... 
    };

}

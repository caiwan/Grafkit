#pragma once

// Based on
// http://www.codeproject.com/Tips/495191/Serialization-implementation-in-Cplusplus
// TODO: Naming has noting to do with the expected behaviour behind 'clone', so please rename it later on

#include <type_traits>
#include <algorithm>
#include <memory>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
//#include <string>
//#include <exception>
#include <functional>
#include <cassert>

#include "core/asset.h"

// https://stackoverflow.com/questions/41853159/how-to-detect-if-a-type-is-shared-ptr-at-compile-time
template <typename T>
struct is_shared_ptr : std::false_type
{
};

template <typename T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type
{
};

template <typename T>
constexpr bool is_shared_ptr_v = is_shared_ptr<T>::value;

namespace Grafkit
{
    class IStream;
    class Serializable;
    class Archive;
    class Peristence;

    class AddFactory;

    class Peristence
    {
        friend class AddFactory;

    public:
        typedef std::function<Serializable*()> FactoryFunction;

        template <class T>
        static void Load(Archive& ar, T*& obj);

        template <class T>
        static void Store(const Archive& ar, T* const & obj);

        template <class T>
        static void Load(Archive& ar, std::shared_ptr<T>& obj);

        template <class T>
        static void Store(const Archive& ar, const std::shared_ptr<T>& obj);

        static Peristence& Instance()
        {
            static Peristence instance;
            return instance;
        }

        Serializable* Create(const char* className);

    protected:
        void AddClonable(const char*const& className, const FactoryFunction factory) { m_factories[className] = factory; }

    private:
        Peristence() = default;
        Peristence(const Peristence&) = delete;
        Peristence& operator=(const Peristence&) = delete;
        ~Peristence() = default;

        std::map<std::string, FactoryFunction> m_factories;

    public:
        // Helper that adds dynmaics factory in compile time
        template <class CLAZZ>
        class AddFactory
        {
        public:
            AddFactory(const char* clazzName) { Instance().AddClonable(clazzName, []() { return new CLAZZ(); }); }
        };
    };

    namespace EndianSwapper
    {
        class SwapByteBase
        {
        public:
            static bool ShouldSwap()
            {
                static const uint16_t swapTest = 1;
                return (*((char*)&swapTest) == 1);
            }

            static void SwapBytes(uint8_t& v1, uint8_t& v2)
            {
                uint8_t tmp = v1;
                v1 = v2;
                v2 = tmp;
            }
        };

        template <class T, int S>
        class SwapByte : public SwapByteBase
        {
        public:
            static T Swap(T v)
            {
                assert(false); // Shoud not be here...
                return v;
            }
        };

        template <class T>
        class SwapByte<T, 1> : public SwapByteBase
        {
        public:
            static T Swap(T v) { return v; }
        };

        template <class T>
        class SwapByte<T, 2> : public SwapByteBase
        {
        public:
            static T Swap(T v)
            {
                if (ShouldSwap())
                    return (v >> 8) | (v << 8);
                return v;
            }
        };

        template <class T>
        class SwapByte<T, 4> : public SwapByteBase
        {
        public:
            static T Swap(T v)
            {
                if (ShouldSwap()) { return (SwapByte<uint16_t, 2>::Swap(uint32_t(v) & 0xffff) << 16) | (SwapByte<uint16_t, 2>::Swap((uint32_t(v) & 0xffff0000) >> 16)); }
                return v;
            }
        };

        template <class T>
        class SwapByte<T, 8> : public SwapByteBase
        {
        public:
            static T Swap(T v)
            {
                if (ShouldSwap())
                    return (uint64_t(SwapByte<uint32_t, 4>::Swap(uint32_t(v & 0xffffffffull))) << 32) | (SwapByte<uint32_t, 4>::Swap(uint32_t(v >> 32)));
                return v;
            }
        };

        template <>
        class SwapByte<float, 4> : public SwapByteBase
        {
        public:
            static float Swap(float v)
            {
                union
                {
                    float f;
                    uint8_t c[4];
                };
                f = v;
                if (ShouldSwap())
                {
                    SwapBytes(c[0], c[3]);
                    SwapBytes(c[1], c[2]);
                }
                return f;
            }
        };

        template <>
        class SwapByte<double, 8> : public SwapByteBase
        {
        public:
            static double Swap(double v)
            {
                union
                {
                    double f;
                    uint8_t c[8];
                };
                f = v;
                if (ShouldSwap())
                {
                    SwapBytes(c[0], c[7]);
                    SwapBytes(c[1], c[6]);
                    SwapBytes(c[2], c[5]);
                    SwapBytes(c[3], c[4]);
                }
                return f;
            }
        };
    }

    // ---- 

    class Archive
    {
    public:

        // ---------------------------------------------------------------------------------------------------------------------------------

        // :(
        Archive(std::unique_ptr<IStream> stream) : m_stream(move(stream)) {
        }

        Archive(const Archive& other) = delete;
        Archive& operator=(const Archive& other) = delete;

        // --- 
        template <class T>
        const Archive& operator<<(const T& v) const
        {
            *this & v;
            return *this;
        }

        template <class T>
        Archive& operator>>(T& v)
        {
            *this & v;
            return *this;
        }


        template <typename T>
        Archive& operator&(T& v)
        {
            if constexpr
                (std::is_pointer_v<T>) {
                Peristence::Load(*this, v);
            }
            else
                if constexpr
                    (is_shared_ptr_v<T>) {
                    Peristence::Load(*this, v);
                }
                else { v.Serialize(*this); }

                return *this;
        }

        template <typename T>

        const Archive& operator&(const T& v) const
        {
            if constexpr
                (std::is_pointer_v<T>) {
                Peristence::Store(*this, const_cast<T&>(v));
            }
            else
                if constexpr
                    (is_shared_ptr_v<T>) {
                    Peristence::Store(*this, v);
                }
                else { const_cast<T&>(v).Serialize(*this); }

                return *this;
        }

        template <class T, size_t N>
        Archive& operator&(T(&v)[N])
        {
            uint32_t len;
            *this & len;
            for (size_t i = 0; i < N; ++i)
                *this & v[i];
            return *this;
        }

        template <class T, size_t N>
        const Archive& operator&(const T(&v)[N]) const
        {
            uint32_t len = N;
            *this & len;
            for (size_t i = 0; i < N; ++i)
                *this & v[i];
            return *this;
        }

        template <class T, size_t N>
        Archive& operator&(std::array<T, N>& v)
        {
            uint32_t len;
            *this & len;
            for (size_t i = 0; i < N; ++i)
                *this & v[i];
            return *this;
        }

        template <class T, size_t N>
        const Archive& operator&(const std::array<T, N>& v) const
        {
            uint32_t len = N;
            *this & len;
            for (size_t i = 0; i < N; ++i)
                *this & v[i];
            return *this;
        }

        /// TODO: add is pod or artihmetic 

#define SERIALIZER_FOR_POD(type) \
        Archive& operator&(type& v) \
        { \
            m_stream->Read((char*)&v, sizeof(type)); \
            if(!m_stream->IsSuccess()) { \
                throw std::runtime_error("malformed data"); \
            } \
            v = Swap(v); \
            return *this; \
        } \
        const Archive& operator&(type v) const \
        { \
            v = Swap(v); \
            m_stream->Write((const char*)&v, sizeof(type)); \
            return *this; \
        }

        SERIALIZER_FOR_POD(bool);
        SERIALIZER_FOR_POD(char);
        SERIALIZER_FOR_POD(unsigned char);
        SERIALIZER_FOR_POD(short);
        SERIALIZER_FOR_POD(unsigned short);
        SERIALIZER_FOR_POD(int);
        SERIALIZER_FOR_POD(unsigned int);
        SERIALIZER_FOR_POD(long);
        SERIALIZER_FOR_POD(unsigned long);
        SERIALIZER_FOR_POD(long long);
        SERIALIZER_FOR_POD(unsigned long long);
        SERIALIZER_FOR_POD(float);
        SERIALIZER_FOR_POD(double);


#define SERIALIZER_FOR_STL(type) \
        template <class T> \
        Archive& operator&(type<T>& v) \
        { \
            uint32_t len; \
            *this & len; \
            v.clear(); \
            for(uint32_t i = 0; i < len; ++i) \
            { \
                T value; \
                *this & value; \
                v.insert(v.end(), value); \
            } \
            return *this; \
        } \
        template <class T> \
        const Archive& operator&(const type<T>& v) const \
        { \
            uint32_t len = v.size(); \
            *this & len; \
            for(typename type<T>::const_iterator it = v.begin(); it != v.end(); ++it) \
                *this & *it; \
            return *this; \
        }

#define SERIALIZER_FOR_STL2(type) \
        template <class T1, class T2> \
        Archive& operator&(type<T1, T2>& v) \
        { \
            uint32_t len; \
            *this & len; \
            v.clear(); \
            for(uint32_t i = 0; i < len; ++i) \
            { \
                std::pair<T1, T2> value; \
                *this & value; \
                v.insert(v.end(), value); \
            } \
            return *this; \
        } \
        template <class T1, class T2> \
        const Archive& operator&(const type<T1, T2>& v) const \
        { \
            uint32_t len = v.size(); \
            *this & len; \
            for(typename type<T1, T2>::const_iterator it = v.begin(); it != v.end(); ++it) \
            *this & *it; \
            return *this; \
        }

        SERIALIZER_FOR_STL(std::vector);
        SERIALIZER_FOR_STL(std::deque);
        SERIALIZER_FOR_STL(std::list);
        SERIALIZER_FOR_STL(std::set);
        SERIALIZER_FOR_STL(std::multiset);
        SERIALIZER_FOR_STL2(std::map);
        SERIALIZER_FOR_STL2(std::multimap);

        template <class T1, class T2>
        Archive& operator&(std::pair<T1, T2>& v)
        {
            *this & v.first & v.second;
            return *this;
        }

        template <class T1, class T2>
        const Archive& operator&(const std::pair<T1, T2>& v) const
        {
            *this & v.first & v.second;
            return *this;
        }

        Archive& operator&(std::string& v)
        {
            uint32_t len;
            *this & len;
            v.clear();
            char buffer[4096];
            uint32_t toRead = len;
            while (toRead != 0)
            {
                uint32_t l = std::min(toRead, uint32_t(sizeof(buffer)));
                m_stream->Read(buffer, l);
                if (!m_stream->IsSuccess())
                    throw std::runtime_error("malformed data");
                v += std::string(buffer, l);
                toRead -= l;
            }
            v.pop_back(); // rm last trailing 
            return *this;
        }

        const Archive& operator&(const std::string& v) const
        {
            uint32_t len = uint32_t(v.length() + 1);
            *this & len;
            m_stream->Write(v.c_str(), len);
            return *this;
        }


    private:
        template <class T>
        T Swap(const T& v) const { return EndianSwapper::SwapByte<T, sizeof(T)>::Swap(v); }

        // --- 
        std::unique_ptr<IStream> m_stream;
        //Ref<IStream> m_stream;
    };

    class Serializable
    {
        friend class Peristence;
    public:
        // --- 
        Serializable() = default;
        virtual ~Serializable() = default;

    protected:
        // --- 
        virtual std::string GetClazzName() const = 0; // collision w/ Winapi macro
        virtual uint16_t GetVersion() const = 0;

        virtual void _InvokeSerialize(const Archive& ar) { throw std::runtime_error("Not implemented"); };

        virtual void _InvokeDeserialize(Archive&) { throw std::runtime_error("Not implemented"); };
    };
}

// TODO: Move to .inl && needs some more cleaning || briong dynamics and persisntence back 
inline Grafkit::Serializable* Grafkit::Peristence::Create(const char* className)
{
    auto it = m_factories.find(className);
    if (it == m_factories.end())
        return nullptr;
    Serializable* serializable = it->second();
    return serializable;
}

template <class T>
void Grafkit::Peristence::Load(Archive& ar, T*& obj)
{
    assert(!obj);
    std::string clazzName;
    uint16_t version = 0;
    ar >> clazzName >> version;
    if (clazzName.empty())
    {
        obj = nullptr;
        return;
    }
    Serializable* const p = (Instance().Create(clazzName.c_str()));
    T* d = dynamic_cast<T*>(p);
    if (!p || !d)
        throw std::runtime_error("Cannot instantitae class: Given <T> is not Serializable or defiend for dynamics");
    if (version != p->GetVersion())
        throw std::runtime_error("Version mismatch");
    p->_InvokeDeserialize(ar);
    obj = d;
}

template <class T>
void Grafkit::Peristence::Store(const Archive& ar, T* const & obj)
{
    if (!obj)
    {
        // inidcate that ptr was null
        ar << "" << uint16_t(0);
        return;
    }
    auto d = dynamic_cast<Serializable*>(obj);
    const std::string clazzName = d->GetClazzName();
    const uint16_t version = d->GetVersion();
    ar << clazzName << version;
    d->_InvokeSerialize(ar);
}

template <class T>
void Grafkit::Peristence::Load(Archive& ar, std::shared_ptr<T>& obj)
{
    T * tp = nullptr; 
    Load(ar, tp);
    obj = std::shared_ptr<T>(obj, tp);
}

template <class T>
void Grafkit::Peristence::Store(const Archive& ar, const std::shared_ptr<T> & obj)
{
    T * tp = obj.get();
    Store(ar, tp);
}



// ---- 

// TODO: https://stackoverflow.com/questions/7582546/using-generic-stdfunction-objects-with-member-functions-in-one-class
// TODO: Merge macros later

#define DYNAMICS_DECL(CLAZZ) \
private: \
	static Grafkit::Peristence::AddFactory<CLAZZ> __addDynamicsFactory;

#define DYNAMICS_IMPL(CLAZZ) \
	Grafkit::Peristence::AddFactory<CLAZZ> CLAZZ##::__addDynamicsFactory(#CLAZZ)

#define PERSISTENT_DECL(CLAZZ, VERSION) \
    DYNAMICS_DECL(CLAZZ) \
protected:\
	std::string GetClazzName() const override { return #CLAZZ; } \
	uint16_t GetVersion() const override { return VERSION; }\
    void _InvokeSerialize(const Grafkit::Archive &ar) override {Serialize(ar);} \
    void _InvokeDeserialize(Grafkit::Archive &ar) override {Serialize(ar);}

// for some convinience
#define SERIALIZE(CLAZZ, VERSION, myAr) \
    PERSISTENT_DECL(CLAZZ, VERSION) \
public: \
    template <class AR> void Serialize(AR &myAr)

#define PERSISTENT_IMPL(CLAZZ) \
    DYNAMICS_IMPL(CLAZZ)

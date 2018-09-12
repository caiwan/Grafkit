#pragma once

#include <vector>

#include "math/matrix.h"

#include "core/reference.h"

#include "render/Actor.h"

namespace Grafkit
{
    class Animation;
    typedef Ref<Animation> AnimationRef;

    /**
    Common anim. interface
    */
    class Animation : public Object 
    {
    public:
        Animation();

        virtual ~Animation();

        // Avoid call Initialization when you deserialize 
        virtual void Initialize() = 0;
        virtual void Update(double time) = 0;

        struct Key;
        class Channel;
        class Track;

        typedef Ref<Channel> ChannelRef;
        typedef Ref<Track> TrackRef;

    protected:
        void Serialize(Archive& ar) override = 0;
        void _Serialize(Archive& ar) override;

    public:

        enum KeyInterpolation_e
        {
            KI_step,
            KI_linear,
            KI_ramp,
            KI_smooth,
            KI_hermite,
            KI_COUNT
        };

        /** Tamplate for anim keys */
        struct Key
        {
            Key() : m_time(0)
                , m_value(0.)
                , m_radius(1.)
                , m_angle(0.)
                , m_type(KI_linear) {
            }

            Key(float t, float value) : m_time(t)
                , m_value(value)
                , m_radius(1.)
                , m_angle(0.)
                , m_type(KI_linear) {
            }

            Key(Key const& other) : m_time(other.m_time)
                , m_value(other.m_value)
                , m_radius(other.m_radius)
                , m_angle(other.m_angle)
                , m_type(other.m_type) {
            }

            Key(float time, float value, float radius, float angle, KeyInterpolation_e type)
                : m_time(time)
                , m_value(value)
                , m_radius(radius)
                , m_angle(angle)
                , m_type(type) {
            }

            float m_time;
            float m_value;
            float m_radius;
            float m_angle;

            KeyInterpolation_e m_type;
        };

        /**
            Template for animation track
        */
        class Channel : public Referencable, public Persistent
        {
            friend class Animation;
        public:
            Channel();
            explicit Channel(const char* name);
            explicit Channel(const std::string& name);
            explicit Channel(const ChannelRef& other);

            Channel(const Channel& other);

            size_t GetKeyCount() const { return m_keys.size(); }
            Key GetKey(size_t i) { return m_keys[i]; }

            float GetValue(const float& time) const;
            void SetValue(const size_t& id, const float& value);

            void AddKey(const Key key) { m_keys.push_back(key); }
            void SetKey(const size_t id, const Key key) { m_keys[id] = key; }

            void InsertKey(const size_t& beforeId, const Key& key);

            void DeleteKey(const size_t& id);

            int FindKeyIndex(float t) const;

            std::string GetName() const { return m_name; }

            /** Finds a key inside the track
            @return 1 if key found
            */
            int FindKey(float t, Key& v0, Key& v1, float& f) const;

            void serialize(Archive& ar);
            void Clear() { m_keys.clear(); }

            void CopyKey(float t, Channel& other);
            void CopyTo(const ChannelRef& target, size_t start, size_t end);

        protected:
            std::vector<Key> m_keys;
            std::string m_name;

            PERSISTENT_DECL(Grafkit::Animation::Channel, 1);
        protected:
            void Serialize(Archive& ar) override;
        };

        /**
            N-dimension container for multiple tracks
        */
        class Track : virtual public Referencable, public Persistent
        {
            friend class Animation;
        public:
            Track();
            explicit Track(const char* name);
            explicit Track(const char* name, const char* channelInitals);
            explicit Track(const char* name, const std::vector<std::string> channelNames);
            ~Track() = default;

            size_t CreateChannel(const char* name);
            size_t CreateChannel(std::string name);

            void SetChannel(const size_t subId, Ref<Channel>& track) { m_channels[subId] = track; }
            Ref<Channel> GetChannel(const size_t subId) { return m_channels[subId]; }

            // TODO: independent, generic getter and setter for keys and values
            float3 GetFloat3(float t) const;
            float4 GetFloat4(float t) const;

            void SetFloat3(size_t id, float3 v);
            void SetFloat4(size_t id, float4 v);

            size_t GetChannelCount() const { return m_channels.size(); }

            std::string GetName() const { return m_name; }
            void SetName(std::string name) { m_name = name; }

        protected:
            std::string m_name;
            std::vector<Ref<Channel>> m_channels;

            PERSISTENT_DECL(Grafkit::Animation::Track, 1)

        protected:
            void Serialize(Archive& ar) override;
        };

        // -- Animation

        Ref<Track> GetTrack(size_t id) { return m_tracks[id]; }
        size_t GetTrackCount() const { return m_tracks.size(); }

    protected:
        void AddTrack(Ref<Track> track) { m_tracks.push_back(track); }

        std::vector<Ref<Track>> m_tracks;
    };
}

/* ============================================================================================== */
namespace Grafkit
{
    inline Animation::Channel::Channel() { m_keys.push_back(Key()); }

    inline Animation::Channel::Channel(const char* name) : m_name(name) { m_keys.push_back(Key()); }

    inline Animation::Channel::Channel(const std::string& name) : m_name(name) { m_keys.push_back(Key()); }

    inline Animation::Channel::Channel(const Channel& other) { for (size_t i = 0; i < other.m_keys.size(); i++) { m_keys.push_back(Key(other.m_keys.at(i))); } }

    inline Animation::Channel::Channel(const ChannelRef& other) : m_name(other->m_name) { for (size_t i = 0; i < other->m_keys.size(); i++) { m_keys.push_back(Key(other->m_keys.at(i))); } }

    inline float Animation::Channel::GetValue(const float& time) const
    {
        float t = 0.f;
        Key v0, v1, v;
        FindKey(time, v0, v1, t);

        switch (v0.m_type)
        {
        case KI_step:
            return v0.m_value;
        case KI_linear:
            break;
        case KI_ramp:
            t = t * t;
            break;
        case KI_smooth:
            t = t * t * (3 - 2 * t);
            break;
        case KI_hermite:
            // See: https://en.wikipedia.org/wiki/Cubic_Hermite_spline#Interpolation_on_a_single_interval
            //const float tangent = v1.m_value - v0.m_value;

            const float t2 = t * t;
            const float t3 = t2 * t;

            return (
                (2.0f * t3 - 3.0f * t2 + 1.0f) * v0.m_value +
                (-2.0f * t3 + 3.0f * t2) * v1.m_value +
                (t3 - 2.0f * t2 + t) * v0.m_angle * v0.m_radius +
                (t3 - t2) * v1.m_angle * v1.m_radius
            );
        }

        return v0.m_value * (1.f - t) + v1.m_value * t;
    }

    inline void Animation::Channel::SetValue(const size_t& id, const float& v) { m_keys[id].m_value = v; }

    inline int Animation::Channel::FindKeyIndex(float t) const
    {
        const int count = m_keys.size();
        if (count == 0)
            return -1;

        if (m_keys[0].m_time > t)
            return -1;

        if (m_keys[count - 1].m_time <= t)
            return count - 1;

        int u = count - 1, l = 0, m = 0;
        while (u >= l)
        {
            m = l + (u - l) / 2;
            const float k0 = m_keys[m].m_time;
            const float k1 = m_keys[m + 1].m_time;

            if (k0 <= t && k1 >= t) { return m; }
            if (k0 > t) { u = m - 1; }
            else { l = m + 1; }
        }
        return -1;
    }


    inline int Animation::Channel::FindKey(float t, Key& v0, Key& v1, float& f) const
    {
        const size_t count = m_keys.size();
        f = 0.f;
        if (!count) { return -1; }

        if (count == 1)
        {
            f = 0.;
            v1 = v0 = m_keys[0];
            return 0;
        }

        if (m_keys[0].m_time >= t)
        {
            f = 0.;
            v0 = m_keys[0];
            v1 = m_keys[1];
            return 0;
        }

        if (m_keys[count - 1].m_time <= t)
        {
            f = 1.;
            v0 = m_keys[count - 2];
            v1 = m_keys[count - 1];
            return 0;
        }

        const int i = FindKeyIndex(t);

        if (i == -1)
            return 0;

        const float d = m_keys[i + 1].m_time - m_keys[i].m_time;
        f = (t - m_keys[i].m_time) / d;

        v0 = m_keys[i];
        v1 = m_keys[i + 1];

        return 1;
    }

    inline void Animation::Channel::serialize(Archive& ar)
    {
        size_t len = 0;

        if (ar.IsStoring()) { len = m_keys.size(); }
        else { m_keys.clear(); }

        PERSIST_FIELD(ar, len);

        for (size_t i = 0; i < len; ++i)
        {
            Key key;

            if (ar.IsStoring()) { key = m_keys[i]; }

            PERSIST_FIELD(ar, key.m_time);
            PERSIST_FIELD(ar, key.m_type);
            PERSIST_FIELD(ar, key.m_value);

            if (!ar.IsStoring()) { m_keys.push_back(key); }
        }
    }

    inline void Animation::Channel::CopyKey(float t, Channel& other)
    {
        if (t <= m_keys.front().m_time)
            return other.AddKey(m_keys.front());
        if (t >= m_keys.back().m_time)
            return other.AddKey(m_keys.back());

        const int i = FindKeyIndex(t);
        if (i > -1) { other.AddKey(m_keys[i]); }
    }

    inline void Animation::Channel::CopyTo(const ChannelRef& target, size_t start, size_t end) { for (size_t i = start; i < end; i++) { target->AddKey(m_keys[i]); } }
}

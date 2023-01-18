#pragma once 

#include <vector>
#include <map>
#include <string>

#include "math/matrix.h"
#include "math/quaternion.h"

#include "utils/reference.h"

#include "render/Actor.h"

namespace Grafkit {

	class Animation;
	typedef Ref<Animation> AnimationRef;

	/**
	Common anim. interface
	*/
	class Animation : public virtual Referencable {
	public:
		Animation() {}
		virtual ~Animation() {}

		virtual void Update(double time) = 0;

		void SetName(std::string name) { m_name = name; }
		std::string GetName() const { return m_name; }

	protected:
		void _serialize(Archive &ar);

	private:
		std::string m_name;
		float m_duration;

	public:

		enum KeyInterpolation_e {
			KI_step,
			KI_linear,
			KI_ramp,
			KI_smooth,
			KI_hermite,
			KI_COUNT
		};

		/** Tamplate for anim keys */
		struct Key {
			Key() : m_time(0), m_value(0.), m_type(KI_linear), m_tangent(float2(1, 0)) {}
			Key(float t, float value) : m_time(t), m_value(value), m_type(KI_linear), m_tangent(float2(1, 0)) {}
			Key(Key const &other) : m_time(other.m_time), m_value(other.m_value), m_type(other.m_type), m_tangent(other.m_tangent) {}

			KeyInterpolation_e m_type;

			float m_time;
			float m_value;
			float2 m_tangent;
		};

		/**
			Template for animation track
		*/
		class Channel : public Referencable {
			friend class Animation;
		public:
			explicit Channel(const char* name = "");
			explicit Channel(const std::string &name);
			Channel(Channel &other);

			size_t GetKeyCount() const { return m_keys.size(); }
			Key GetKey(size_t i) { return m_keys[i]; }

			float GetValue(float time) const;
			void SetValue(size_t id, float value);

			void AddKey(const Key key) { m_keys.push_back(key); }
			void SetKey(const size_t id, const Key key) { m_keys[id] = key; }
			void InsertKey(const size_t afterId, const Key key) { const auto it = m_keys.begin() + afterId; m_keys.insert(it, key); }
			void DeleteKey(const size_t id, Key key) { const auto it = m_keys.begin() + id; m_keys.erase(it); }

			int FindKeyIndex(float t) const;

			std::string GetName() const { return m_name; }

			/** Finds a key inside the track
			@return 1 if key found
			*/
			int FindKey(float t, Key& v0, Key& v1, float &f) const;

		public:
			void serialize(Archive &ar);
			void Clear() { m_keys.clear(); }

			void CopyKey(float t, Channel& other);


		protected:
			std::vector<Key> m_keys;
			std::string m_name;
		};

		/**
			N-dimension container for multiple tracks
		*/
		class Track : virtual public Referencable
		{
			friend class Animation;
		public:
			explicit Track(const char* name = nullptr, const char* channelInitals = nullptr);
			explicit Track(const char* name, const std::vector<std::string> channelNames);
			~Track() = default;

		    size_t CreateChannel(const char* name);
			size_t CreateChannel(std::string name);

			void SetChannel(const size_t subId, Ref<Channel> &track) { m_channels[subId] = track; }
			Ref<Channel> GetChannel(const size_t subId) { return m_channels[subId]; }


			// TODO: independent, generic getter and setter for keys and values
			float3 GetFloat3(float t);
			float4 GetFloat4(float t);

			void SetFloat3(size_t id, float3 v);
			void SetFloat4(size_t id, float4 v);

			size_t GetChannelCount() { return m_channels.size(); }

			std::string GetName() { return m_name; }
			void SetName(std::string name) { m_name = name; }

		protected:
			std::string m_name;
			std::vector<Ref<Channel>> m_channels;
		};

	public:
		Ref<Track> GetTrack(size_t id) { return m_tracks[id]; }
		size_t GetTrackCount() { return m_tracks.size(); }

	protected:
		void AddTrack(Ref<Track> track) { m_tracks.push_back(track); }

	protected:
		std::vector<Ref<Track>> m_tracks;

	};

}

/* ============================================================================================== */
namespace Grafkit {
	inline Animation::Channel::Channel(const char * name) : m_name(name)
	{
		m_keys.push_back(Key());
	}

	inline Animation::Channel::Channel(const std::string &name) : m_name(name)
	{
		m_keys.push_back(Key());
	}

	inline Animation::Channel::Channel(Channel & other)
	{
		for (size_t i = 0; i < other.m_keys.size(); i++) {
			m_keys.push_back(Key(other.m_keys.at(i)));
		}
	}

	inline float Animation::Channel::GetValue(const float time) const
	{
		float t = 0.f;
		Key v0, v1, v;
		FindKey(time, v0, v1, t);

		switch (v0.m_type) {
		case KI_step:
			return v0.m_value;
		case KI_linear:
			break;
		case KI_ramp:
			t = t * t;
			break;
		case KI_smooth:
			t = t * t * (2 - 3 * t);
			break;
		case KI_hermite:
			const float tangent = v1.m_value - v0.m_value;

			const float t2 = t * t;
			const float t3 = t2 * t;

			return (
				(2.0f*t3 - 3.0f*t2 + 1.0f) * v0.m_value +
				(-2.0f*t3 + 3.0f*t2) * v1.m_value +
				(t3 - 2.0f*t2 + t) * v0.m_tangent.x * tangent +
				(t3 - t2) * v1.m_tangent.y * tangent
				);
		}

		return v0.m_value * (1. - t) + v1.m_value * t;
	}

	inline void Animation::Channel::SetValue(size_t id, float v)
	{
		m_keys[id].m_value = v;
	}

	inline int Animation::Channel::FindKeyIndex(float t) const
	{
	    const size_t count = m_keys.size();
		if (count <= 2)
			return 0;

		if (m_keys[0].m_time > t)
			return 0;
		else if (m_keys[count - 1].m_time < t)
			return count - 1;

#if 1
		size_t u = count - 1, l = 0, m = 0;
		while (u >= l) {
			m = l + (u - l) / 2;
		    const float k0 = m_keys[m].m_time;
		    const float k1 = m_keys[m + 1].m_time;

			if (k0 <= t && k1 >= t) {
				return m;
			}
			if (k0 > t) {
				u = m - 1;
			}
			else {
				l = m + 1;
			}
		}
		return -1;

#else 
		for (size_t i = 0; i < count - 1; i++) {
			if (m_keys[i].m_time <= t && m_keys[i + 1].m_time >= t)
				return i;
		}

		return -1;
#endif

	}


	inline int Animation::Channel::FindKey(float t, Key & v0, Key & v1, float & f) const
	{
	    const size_t count = m_keys.size();
		f = 0.f;
		if (!count) {
			return 0;
		}

		if (count == 1)
		{
			f = 0.;
			v1 = v0 = m_keys[0];
			return 0;
		}

		if (m_keys[0].m_time >= t) {
			f = 0.;
			v0 = m_keys[0];
			v1 = m_keys[1];
			return 0;
		}
		else if (m_keys[count - 1].m_time <= t) {
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

	inline void Animation::Channel::serialize(Archive & ar)
	{
		size_t len = 0;

		if (ar.IsStoring()) {
			len = m_keys.size();
		}
		else {
			m_keys.clear();
		}

		PERSIST_FIELD(ar, len);

		for (size_t i = 0; i < len; ++i) {
			Key key;

			if (ar.IsStoring()) {
				key = m_keys[i];
			}

			PERSIST_FIELD(ar, key.m_time);
			PERSIST_FIELD(ar, key.m_type);
			PERSIST_FIELD(ar, key.m_value);

			if (!ar.IsStoring()) {
				m_keys.push_back(key);
			}
		}
	}

	inline void Animation::Channel::CopyKey(float t, Channel & other)
	{
		if (t <= m_keys.front().m_time)
			return other.AddKey(m_keys.front());
		if (t >= m_keys.back().m_time)
			return other.AddKey(m_keys.back());

	    const int i = FindKeyIndex(t);
		if (i > -1) {
			other.AddKey(m_keys[i]);
		}
	}
}


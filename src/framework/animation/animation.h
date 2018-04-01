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
			Key() : m_key(0), m_value(), m_type(KI_linear), m_tangent(0.) {}
			Key(float t, float value) : m_key(t), m_value(value), m_type(KI_linear), m_tangent(0.) {}
			Key(Key const &other) : m_key(other.m_key), m_value(other.m_value), m_type(other.m_type), m_tangent(other.m_tangent) {}

			enum KeyInterpolation_e m_type;

			float m_key;
			float m_value;
			float m_tangent;
		};

		/**
			Template for animation track
		*/
		class Channel : public Referencable {
			friend class Animation;
		public:

			Channel() {}
			Channel(std::string name) : m_name(name) {}
			Channel(Channel &other);

			size_t GetKeyCount() { return m_track.size(); }
			Key GetKey(size_t i) { return m_track[i]; }

			float GetValue(float time);

			void AddKey(Key key) { m_track.push_back(key); }
			void SetKey(Key key, size_t id) { m_track[id] = key; }
			void InsertKey(Key key, size_t afterId) { auto it = m_track.begin() + afterId; m_track.insert(it, key); }
			void DeleteKey(Key key, size_t id) { auto it = m_track.begin() + id; m_track.erase(it); }

			int FindKeyIndex(float t) const;

			std::string GetName() { return m_name; }

			/** Finds a key inside the track
			@return 1 if key found
			*/
			// legacy shit 
			int FindKey(float t, float& v0, float& v1, float &f) const {
				Key k0, k1;
				int r = FindKey(t, k0, k1, f);
				v0 = k0.m_value;
				v1 = k1.m_value;
				return r;
			}

			/** Finds a key inside the track
			@return 1 if key found
			*/
			int FindKey(float t, Key& v0, Key& v1, float &f) const;

		public:
			void serialize(Archive &ar);
			void Clear() { m_track.clear(); }

			void CopyKey(float t, Channel& other);


		protected:
			std::vector<Key> m_track;
			std::string m_name;
		};

		/**
			N-dimension container for multiple tracks
		*/
		class Track : virtual public Referencable
		{
			friend class Animation;
		public:
			Track(std::string name = "") : m_name(name) {
			}

			~Track() {}

			size_t CreateChannel(std::string name) { m_channels.push_back(new Channel(name)); return m_channels.size() - 1; }

			void SetChannel(size_t subId, Ref<Channel> &track) { m_channels[subId] = track; }
			Ref<Channel> GetChannel(size_t subId) { return m_channels[subId]; }

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


	/* ============================================================================================== */

	inline Animation::Channel::Channel(Channel & other)
	{
		for (size_t i = 0; i < other.m_track.size(); i++) {
			m_track.push_back(Key(other.m_track.at(i)));
		}
	}

	inline float Animation::Channel::GetValue(float time)
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
				(t3 - 2.0f*t2 + t) * v0.m_tangent * tangent +
				(t3 - t2) * v1.m_tangent * tangent
				);
		}

		return v0.m_value * (1. - t) + v1.m_value * t;
	}

	inline int Animation::Channel::FindKeyIndex(float t) const
	{
		size_t count = m_track.size();
		if (count <= 2)
			return 0;

		if (m_track[0].m_key > t)
			return 0;
		else if (m_track[count - 1].m_key < t)
			return count - 1;

#if 1
		size_t u = count - 1, l = 0, m = 0;
		while (u >= l) {
			m = l + (u - l) / 2;
			float k0 = m_track[m].m_key;
			float k1 = m_track[m + 1].m_key;

			if (k0 <= t && k1 >= t) {
				return m;
			}
			else if (k0 > t) {
				u = m - 1;
			}
			else {
				l = m + 1;
			}
		}
		return -1;

#else 
		for (size_t i = 0; i < count - 1; i++) {
			if (m_track[i].m_key <= t && m_track[i + 1].m_key >= t)
				return i;
		}

		return -1;
#endif

	}


	inline int Animation::Channel::FindKey(float t, Key & v0, Key & v1, float & f) const
	{
		size_t count = m_track.size();
		f = 0.f;
		if (!count) {
			return 0;
		}

		if (m_track[0].m_key > t) {
			f = 0.;
			v0 = m_track[0];
			v1 = m_track[1];
			return 0;
		}
		else if (m_track[count - 1].m_key < t) {
			f = 1.;
			v0 = m_track[count - 2];
			v1 = m_track[count - 1];
			return 0;
		}

		int i = FindKeyIndex(t);

		if (i == -1)
			return 0;

		float d = m_track[i + 1].m_key - m_track[i].m_key;
		f = (t - m_track[i].m_key) / d;

		v0 = m_track[i];
		v1 = m_track[i + 1];

		return 1;
	}

	inline void Animation::Channel::serialize(Archive & ar)
	{
		size_t len = 0;

		if (ar.IsStoring()) {
			len = m_track.size();
		}
		else {
			m_track.clear();
		}

		PERSIST_FIELD(ar, len);

		for (size_t i = 0; i < len; ++i) {
			Key key;

			if (ar.IsStoring()) {
				key = m_track[i];
			}

			PERSIST_FIELD(ar, key.m_key);
			PERSIST_FIELD(ar, key.m_type);
			PERSIST_FIELD(ar, key.m_value);

			if (!ar.IsStoring()) {
				m_track.push_back(key);
			}
		}
	}

	inline void Animation::Channel::CopyKey(float t, Channel & other)
	{
		if (t <= m_track.front().m_key)
			return other.AddKey(m_track.front());
		if (t >= m_track.back().m_key)
			return other.AddKey(m_track.back());

		int i = FindKeyIndex(t);
		if (i > -1) {
			other.AddKey(m_track[i]);
		}
	}
}


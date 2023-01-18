#pragma once 

#include <vector>
#include <string>

#include "../stdafx.h"
#include "../math/matrix.h"
#include "../math/quaternion.h"

#include "Actor.h"

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

		void SetDuration(float d) { m_duration = d; }
		float GetDuration() { return m_duration; }

		//void CalcDuration(bool isMin = false);

		virtual void Clear() = 0;
		virtual void CopyKey(float t, Animation * const & other) = 0;

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
			KI_catmullRom,
			KI_COUNT
		};

		/** Tamplate for anim keys */
		template <typename V> struct Key {

			Key() : m_key(0), m_value(), m_type(KI_linear) {}
			Key(float t, V value) : m_key(t), m_value(value), m_type(KI_linear) {}

			float m_key;
			V m_value;
			enum KeyInterpolation_e m_type;
		};

		/**
			Template for animation track
		*/
		template <typename V> class Track {
			friend class Animation;
		public:

			typedef V value_t;

			Track() {}

			void AddKey(Key<V> key) {
				m_track.push_back(key);
			}

			Key<V> GetKey(size_t i) {
				return m_track[i];
			}

			size_t GetKeyCount() {
				return m_track.size();
			}

			int FindKeyIndex(float t) const {
				size_t count = m_track.size();
				if (count <= 2)
					return 0;
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

			/** Finds a key inside the track
			@return 1 if key found
			*/
			int FindKey(float t, V& v0, V& v1, float &f) const {
				size_t count = m_track.size();
				f = 0.f;
				if (!count) {
					return 0;
				}

				if (m_track[0].m_key > t) {
					f = 0.;
					v0 = m_track[0].m_value;
					v1 = m_track[1].m_value;
					return 0;
				}
				else if (m_track[count - 1].m_key < t) {
					f = 1.;
					v0 = m_track[count - 2].m_value;
					v1 = m_track[count - 1].m_value;
					return 0;
				}

				int i = FindKeyIndex(t);

				if (i == -1)
					return 0;

				float d = m_track[i + 1].m_key - m_track[i].m_key;
				f = (t - m_track[i].m_key) / d;

				v0 = m_track[i].m_value;
				v1 = m_track[i + 1].m_value;

				return 1;
			}

		public:
			void serialize(Archive &ar);
			void Clear() { m_track.clear(); }

			void CopyKey(float t, Track<V>& other) {
				if (t <= m_track.front().m_key)
					return other.AddKey(m_track.front());
				if (t >= m_track.back().m_key)
					return other.AddKey(m_track.back());

				int i = FindKeyIndex(t);
				if (i > -1) {
					other.AddKey(m_track[i]);
				}
			}

		public:
			std::vector<Key<V>> m_track;
		};

		/* Predefs */
	public:
		typedef Animation::Key<float> FloatKey;
		typedef Animation::Key<float2> Vector2Key;
		typedef Animation::Key<float3> Vector3Key;
		typedef Animation::Key<float4> Vector4Key;

		typedef Animation::Track<float> FloatTrack;
		typedef Animation::Track<float2> Vector2Track;
		typedef Animation::Track<float3> Vector3Track;
		typedef Animation::Track<float4> Vector4Track;

	protected:
		static void FindKey(FloatTrack track, double &time, float &value);
		static void FindKey(Vector3Track track, double &time, float3 &value);
		static void FindKey(Vector4Track, double &time, Quaternion &value);

		//template<typename T> static void persistTrack(Archive& ar, Track<T> &track);
	};



	/* ============================================================================================== */

	/**
	Animation for scenegraph nodes
	*/
	class ActorAnimation : public Animation, public Persistent {
	public:
		ActorAnimation() : Animation() {}
		~ActorAnimation() { }

		//void Initialize(ActorRef actor);

		void SetActor(ActorRef &actor) { m_actor = actor; }
		ActorRef GetActor() { return m_actor; }

		void AddPositionKey(float key, float3 value) { m_positionTrack.AddKey(Vector3Key(key, value)); }
		void AddScalingKey(float key, float3 value) { m_scalingTrack.AddKey(Vector3Key(key, value)); }
		void AddRotationKey(float key, float4 value) { m_rotationTrack.AddKey(Vector4Key(key, value)); }

		virtual void Update(double time);
		virtual void Clear();
		virtual void CopyKey(float f, Animation * const & other);

	protected:
		ActorRef m_actor;

		Vector3Track m_positionTrack;
		Vector3Track m_scalingTrack;
		Vector4Track m_rotationTrack;

	protected:
		virtual void serialize(Archive& ar);
		PERSISTENT_DECL(ActorAnimation, 1);
	};

	/* ============================================================================================== */

	/**
	Animation for entities inside the scenegraph nodes
	*/

	class EntityAnimation : public Animation, public Persistent {
		// .. 
	public:
		EntityAnimation() {}
		~EntityAnimation() {}

		void SetEntity(Ref<Entity3D> & entity) { m_entity = entity; }
		Ref<Entity3D> GetEntity() { return m_entity; }

		virtual void Update(double time) = 0;

	protected:
		Ref<Entity3D> m_entity;

	};

	/** Qnd Camera fov animation
	*/

	class CameraAnimation : public EntityAnimation {
	public:
		void Update(double t);
		void Clear();
		void CopyKey(float f, Animation * const & other);


		void AddFovKey(float key, float value) { m_fov.AddKey(FloatKey(key, value)); }

	private:
		FloatTrack m_fov;

	protected:
		virtual void serialize(Archive& ar);
		PERSISTENT_DECL(CameraAnimation, 1);
	};

	/* ============================================================================================== */

	template<typename V>
	inline void Animation::Track<V>::serialize(Archive & ar)
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
			Key<value_t> key;

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

}


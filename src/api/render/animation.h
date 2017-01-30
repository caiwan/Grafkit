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

		void CalcDuration(bool isMin = false);

	protected:
		void _serialize(Archive &ar);

	private: 
		std::string m_name;
		float m_duration;

	public:

		enum KeyInterpolation_e{
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

			Track(){}

			void AddKey(Key<V> key) { 
				m_track.push_back(key); 
			}

			/** Finds a key inside the track
			@return 1 if key found
			*/
			int findKey(float t, V& v0, V& v1, float &f) const {
				size_t count = m_track.size();
				f = 0.f;
				if (!count) {
					return 0;
				}
						
				if (count == 1 || m_track[0].m_key > t) {
					v1 = v0 = m_track[0].m_value;
					return 1;
				}
				
				for (size_t i = 0; i < count - 1; i++) {
					if (m_track[i].m_key <= t && m_track[i + 1].m_key>= t) {
						float d = m_track[i + 1].m_key - m_track[i].m_key;
						f = (t - m_track[i].m_key) / d;
			
						v0 = m_track[i].m_value;
						v1 = m_track[i + 1].m_value;

						return 1;
					}
				}
				v1 = v0 = m_track[count - 1].m_value;
				return 1;
			}

		protected:
			void serialize(Archive &ar);
			


		public:
			std::vector<Key<V>> m_track;
		};

		/* Predefs */
		typedef Animation::Key<float3> Vector3Key;
		typedef Animation::Key<float4> Vector4Key;
		typedef Animation::Key<Quaternion> QuaternionKey;

		typedef Animation::Track<float3> Vector3Track;
		typedef Animation::Track<float4> Vector4Track;
		typedef Animation::Track<Quaternion> QuaternionTrack;

		static void FindKey(Vector3Track track, double &time, float3 &value);
		static void FindKey(QuaternionTrack track, double &time, Quaternion &value);

		template<typename T> static void persistTrack(Archive& ar, Track<T> &track);
	};



	/* ============================================================================================== */

	/** 
	Animation for scenegraph nodes 
	*/
	class ActorAnimation : public Animation, public Persistent{
	public:
		ActorAnimation() : Animation() {}
		~ActorAnimation() { }

		//void Initialize(ActorRef actor);

		void SetActor(ActorRef &actor) { m_actor = actor; }
		ActorRef GetActor() { return m_actor; }

		void AddPositionKey(float key, float3 value) { m_positionTrack.AddKey(Vector3Key(key, value)); }
		void AddScalingKey(float key, float3 value) { m_scalingTrack.AddKey(Vector3Key(key, value)); }
		void AddRotationKey(float key, float4 value) { m_rotationTrack.AddKey(QuaternionKey(key, value)); }
		void AddRotationKey(float key, Quaternion value) { m_rotationTrack.AddKey(QuaternionKey(key, value)); }

		virtual void Update(double time);
	
	protected:
		ActorRef m_actor;

		Vector3Track m_positionTrack;
		Vector3Track m_scalingTrack;
		QuaternionTrack m_rotationTrack;

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
		EntityAnimation(){}
		~EntityAnimation() {}

		void SetEntity(Ref<Entity3D> & entity) { m_entity = entity; }
		Ref<Entity3D> GetEntity() {return m_entity;}

		virtual void Update(double time) = 0;

	protected:
		Ref<Entity3D> m_entity;

	};

}


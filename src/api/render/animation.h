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

		//virtual void Shutdown() = 0;

		virtual void Update(double time) = 0;

		void SetName(std::string name) { m_name = name; }
		std::string GetName() const { return m_name; }

	private: 
		std::string m_name;

	public:

		enum KeyInterpolation_e{
			KI_step,
			KI_linear,
			KI_ramp,
			KI_smooth,
			KI_catmullRom
		};

		/** Tamplate for anim keys */
		template <typename V> class Key {
		public:
			Key() : m_t(0), m_value(), m_type(KI_linear) {}
			Key(float t, V value) : m_t(t), m_value(value), m_type(KI_linear) {}

			// mas nem kell egyelore igaz?

		public:
			float m_t;
			V m_value;
			enum KeyInterpolation_e m_type;
		};

		/** 
			Template for animation track
		*/
		template <typename V> class Track {
		public:
			Track(){}

			void AddKey(Key<V> key) { 
				m_track.push_back(key); 
			}

			/** Finds a key inside the track
			@return 1 if propert key found
			*/
			int findKey(float t, V& v0, V& v1, float &f) const {
				size_t count = m_track.size();
				f = 0.f;
				if (!count) {
					return 0;
				}
						
				if (count == 1 || m_track[0].m_t > t) {
					v1 = v0 = m_track[0].m_value;
					return 1;
				}
				
				for (size_t i = 0; i < count - 1; i++) {
					if (m_track[i].m_t <= t && m_track[i + 1].m_t>= t) {
						float d = m_track[i + 1].m_t - m_track[i].m_t;
						f = (t - m_track[i].m_t) / d;
			
						v0 = m_track[i].m_value;
						v1 = m_track[i + 1].m_value;

						return 1;
					}
				}
				v1 = v0 = m_track[count - 1].m_value;
				return 1;
			}

		public:
			std::vector<Key<V>> m_track;
		};

	};

	/* Predefs */
	typedef Animation::Key<float3> Vector3Key;
	typedef Animation::Key<float4> Vector4Key;
	typedef Animation::Key<Quaternion> QuaternionKey;

	
	typedef Animation::Track<float3> Vector3Track; 
	typedef Animation::Track<float4> Vector4Track;
	typedef Animation::Track<Quaternion> QuaternionTrack;

	/* ============================================================================================== */

	/** 
	Animation for scenegraph nodes 
	*/
	class NodeAnimation : public virtual Animation {
	public:
		NodeAnimation() : Animation() {}
		
		~NodeAnimation() { 
			//Shutdown();
		}

		void Initialize(ActorRef actor);

		void AddPositionKey(float key, float3 value) { m_positionTrack.AddKey(Vector3Key(key, value)); }
		void AddScalingKey(float key, float3 value) { m_scalingTrack.AddKey(Vector3Key(key, value)); }
		void AddRotationKey(float key, float4 value) { m_rotationTrack.AddKey(QuaternionKey(key, value)); }

		//virtual void Shutdown(){}
		virtual void Update(double time);
	
	protected:
		ActorRef m_actor;

		void findKey(Vector3Track track, double &time, float3 &value);
		void findKey(QuaternionTrack track, double &time, Quaternion &value);

		Vector3Track m_positionTrack;
		Vector3Track m_scalingTrack;
		QuaternionTrack m_rotationTrack;
	};
}

#pragma once 

#include "../stdafx.h"

#include "../core/Music.h"
#include "../render/dxtypes.h"

/* <ROCKET SPECIFIC> */
struct sync_track;
struct sync_device;
typedef const sync_track sync_track_t;
/* </ ROCKET SPECIFIC> */

namespace Grafkit{
	class ValueTracker;

	class Timer{
		friend ValueTracker;

		public:
			Timer();
			~Timer();

			void Initialize(Grafkit::MusicResRef music, double lengthMS, double beatPerMin, int rowPerBeat);
			void Shutdown();

			/**
			Csatlakozik a Rockethez.
			A pathnek leteznie kell `./sync/` kulonben nem mukodik, es nem ad vissza hibat
			*/
			void Connect(const char *_ipaddr = NULL);
			/** Updateli a rocketet */
			void Update();

		// synctracker wrapper
			void SetRow(int row) const;
			double GetRowD() const;
			int GetRowI() const;
			
			double GetRowD(double timems) const;
			int GetRowI(double timems) const;

			inline double GetTime() const {return Music()->GetTime();}
			inline double GetTimems() const {return Music()->GetTimems();}
						
			void TogglePause(int flag);
			inline bool GetPauseFlag() const {return m_pauseFlag != 0;}

			inline double GetLengthMS() const {return m_length;}
			inline double GetLength() const {return m_length/1000.;}

			inline void Play() { Music()->Play(); this->m_pauseFlag = 0;}
			inline void Stop() { Music()->Stop(); this->m_pauseFlag = 1;}

			int GetEnd() const;

		private:

			Grafkit::Music* const Music() const { return this->m_music->Get(); }

			Grafkit::MusicResRef m_music;
			int m_pauseFlag;

			double m_length, m_bpm, m_rowPerBeat, m_rowLength;

			sync_device *m_rocket;
			char m_ipaddress[32];
	};
	
	enum val_track_e {
		VTT_bool_switch, // 0 or 1
		VTT_switch,      // integer (clamp down)
		VTT_scalar,      // scalar value
		VTT_velocity,    // valocity value 0..1
		VTT_Float2,        // 2D vector - xy
		VTT_Float3,        // 3D vector - xyz
		VTT_Float4,        // 4D vector - xyzw
		VTT_VCOUNT
	};

	class ValueTracker{

		protected:
			class Track{
			public:
				Track(ValueTracker *parent, val_track_e type, const char *name, const char *vclassName, const char *vname);
				virtual ~Track();

			protected:

				int GetBool(double t);
				int GetSwitch(double t);
				float GetScalar(double t);
				float GetVelocity(double t);
				float2 GetFloat2(double t);
				float3 GetFloat3(double t);
				float4 GetFloat4(double t);

			protected:
				val_track_e m_type;
				ValueTracker *m_parent;
				sync_track_t *m_tacks[4];
			};

		public:
			ValueTracker(Timer* timer);
			~ValueTracker();

			inline const Timer* getTimer(){return this->m_mainTimer;}
			inline void update(){this->m_mainTimer->Update();}

		private:
			Timer *m_mainTimer;

	

			// Track classes
		public:
			/**
				Has boolean value only. 0 or 1
			*/
			class BoolTrack : public Track{
				public:
					BoolTrack(ValueTracker *parent, const char *name, const char *vclassName, const char *vname): 
					  Track(parent, VTT_bool_switch, name, vclassName, vname){}
					virtual ~BoolTrack(){}

					inline int Get(double t){return this->GetBool(t);}
					inline int Get(){return this->GetBool(this->m_parent->m_mainTimer->GetTimems());} // NPR vedelem?
			};
			
			/**
				Has integers only for addressing/indexing or switching between stuff
			*/
			class SwitchTrack : public Track {
				public:
					SwitchTrack(ValueTracker *parent, const char *name, const char *vclassName, const char *vname) : 
						Track(parent, VTT_switch, name, vclassName, vname){}
					virtual ~SwitchTrack(){}

					inline int Get(double t){return this->GetSwitch(t);}
					inline int Get(){return this->GetSwitch(this->m_parent->m_mainTimer->GetTimems());}
			};
			
			/**
				One float scalar value channel, anything goes 
			*/
			class ScalarTrack : public Track {
				public:
					ScalarTrack(ValueTracker *parent, const char *name, const char *vclassName, const char *vname) : 
						Track(parent, VTT_scalar, name, vclassName, vname){}
					virtual ~ScalarTrack(){}

					inline float Get(double t){return this->GetScalar(t);}
					inline float Get(){return this->GetScalar(this->m_parent->m_mainTimer->GetTimems());}
			};

			/**
				Velocity or interpolation track. Between 0 and 1.
			*/
			class VelocityTrack : public Track{
				public:
					VelocityTrack(ValueTracker *parent, const char *name, const char *vclassName, const char *vname): 
						Track(parent, VTT_velocity, name, vclassName, vname){}
					virtual ~VelocityTrack(){}

					inline float Get(double t){return this->GetVelocity(t);}
					inline float Get(){return this->GetVelocity(this->m_parent->m_mainTimer->GetTimems());}
			};

			/**
				Two component vector. Creates two channel.
			*/

			class Float2Track : public Track{
				public:
					Float2Track(ValueTracker *parent, const char *name, const char *vclassName, const char *vname): 
					  Track(parent, VTT_Float2, name, vclassName, vname){}
					virtual ~Float2Track(){}

					inline float2 Get(double t){return this->GetFloat2(t);}
					inline float2 Get(){return this->GetFloat2(this->m_parent->m_mainTimer->GetTimems());}
			};


			/**
				Three component vector. Creates three channel.
			*/
			class Float3Track : public Track{
				public: 
					Float3Track(ValueTracker *parent, const char *name, const char *vclassName, const char *vname): 
					  Track(parent, VTT_Float3, name, vclassName, vname){}
					virtual ~Float3Track(){}

					inline float3 Get(double t){return this->GetFloat3(t);}
					inline float3 Get(){return this->GetFloat3(this->m_parent->m_mainTimer->GetTimems());}
			};

			/**
				Four component vector. Creates four channel.
			*/
			class Float4Track : public Track{
				public: 
					Float4Track(ValueTracker *parent, const char *name, const char *vclassName, const char *vname): 
					  Track(parent, VTT_Float4, name, vclassName, vname){}
					virtual ~Float4Track(){}

					inline float4 Get(double t){return this->GetFloat4(t);}
					inline float4 Get(){return this->GetFloat4(this->m_parent->m_mainTimer->GetTimems());}
			};


			// factories
			inline ValueTracker::BoolTrack* newBoolTrack(const char *name, const char *vclassName, const char *vname = NULL){
				return new ValueTracker::BoolTrack(this, name, vclassName, vname);}

			inline ValueTracker::SwitchTrack* newSwitchTrack(const char *name, const char *vclassName, const char *vname = NULL){
				return new ValueTracker::SwitchTrack(this, name, vclassName, vname);}

			inline ValueTracker::VelocityTrack* newVelocityTrack(const char *name, const char *vclassName, const char *vname = NULL){
				return new ValueTracker::VelocityTrack(this,name, vclassName, vname);}

			inline ValueTracker::ScalarTrack* newScalarTrack(const char *name, const char *vclassName, const char *vname = NULL){
				return new ValueTracker::ScalarTrack(this, name, vclassName, vname);}

			inline ValueTracker::Float2Track* newFloat2Track(const char *name, const char *vclassName, const char *vname){
				return new ValueTracker::Float2Track(this, name, vclassName, vname);}

			inline ValueTracker::Float3Track* newFloat3Track(const char *name, const char *vclassName, const char *vname){
				return new ValueTracker::Float3Track(this, name, vclassName, vname);}

			inline ValueTracker::Float4Track* newFloat4Track(const char *name, const char *vclassName, const char *vname){
				return new ValueTracker::Float4Track(this, name, vclassName, vname);}

	};

	// ... 

};

DEFINE_EXCEPTION(NoRocketDeviceException, 200, "No Rocket device")
DEFINE_EXCEPTION(InvalidTrackNameException, 201, "Invalid track name")


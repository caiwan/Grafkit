#pragma once 

#include "player/player.h"
#include "math/vectors.h"

/* <ROCKET SPECIFIC> */
struct sync_track;
struct sync_device;
typedef const sync_track sync_track_t;
/* </ ROCKET SPECIFIC> */

namespace FWscene{
	class ValueTracker;

	class Timer{
		friend ValueTracker;

		private:
			// TOOD: erre ennel sokkal szebb megoldast kell talalni :)
			FWplayer::Music *music;
			int pauseFlag;

			double length, bpm, rpb, rowLength;

			sync_device *rocket;
			char ipaddress[32];

		public:
			Timer(FWplayer::Music *music, long lengthMS, double beatPerMin, int rowPerBeat);
			~Timer();

		// synctracker wrapper
			void setRow(int row) const;
			double getRowD() const;
			int getRowI() const;
			
			double getRowD(double timems) const;
			int getRowI(double timems) const;

			inline double getTime() const {return this->music->getTime();}
			inline double getTimems() const {return this->music->getTimems();}
						
			void togglePause(int flag);
			inline bool getPauseFlag() const {return this->pauseFlag != 0;}

			inline double getLengthMS() const {return this->length;}
			inline double getLength() const {return this->length/1000.;}

			inline void play() {this->music->play(); this->pauseFlag = 0;}
			inline void stop() {this->music->stop(); this->pauseFlag = 1;}

			int getEnd() const;

			/**
			Csatlakozik a Rockethez.
			A pathnek leteznie kell `./sync/` kulonben nem mukodik, es nem ad vissza hibat
			*/
			void connect(const char *_ipaddr = NULL);
			void update();
			void release();
	};
	
	enum val_track_e {
		VTT_bool_switch, // 0 or 1
		VTT_switch,      // integer (clamp down)
		VTT_scalar,      // scalar value
		VTT_velocity,    // valocity value 0..1
		VTT_vec2,        // 2D vector - xy
		VTT_vec3,        // 3D vector - xyz
		VTT_vec4,        // 4D vector - xyzw
		VTT_VCOUNT
	};

	class ValueTracker{
		private:
			Timer *mainTimer;
		
		protected:
			class Track{
				protected:
					val_track_e type;
					ValueTracker *parent;
					sync_track_t *tacks[4];

					int getBool(double t);
					int getSwitch(double t);
					float getScalar(double t);
					float getVelocity(double t);
					vec2float getVec2(double t);
					vec3float getVec3(double t);
					vec4float getVec4(double t);

				public:
					Track(ValueTracker *parent, val_track_e type, const char *name, const char *vclassName, const char *vname);
					virtual ~Track();
			};

		public:
			ValueTracker(Timer* timer);
			~ValueTracker();

			inline const Timer* getTimer(){return this->mainTimer;}
			inline void update(){this->mainTimer->update();}

			// Track classes

			/**
				Has boolean value only. 0 or 1
			*/
			class BoolTrack : public Track{
				public:
					BoolTrack(ValueTracker *parent, const char *name, const char *vclassName, const char *vname): 
					  Track(parent, VTT_bool_switch, name, vclassName, vname){}
					virtual ~BoolTrack(){}

					inline int get(double t){return this->getBool(t);}
					inline int get(){return this->getBool(this->parent->mainTimer->getTimems());} // NPR vedelem?
			};
			
			/**
				Has integers only for addressing/indexing or switching between stuff
			*/
			class SwitchTrack : public Track {
				public:
					SwitchTrack(ValueTracker *parent, const char *name, const char *vclassName, const char *vname) : 
						Track(parent, VTT_switch, name, vclassName, vname){}
					virtual ~SwitchTrack(){}

					inline int get(double t){return this->getSwitch(t);}
					inline int get(){return this->getSwitch(this->parent->mainTimer->getTimems());}
			};
			
			/**
				One float scalar value channel, anything goes 
			*/
			class ScalarTrack : public Track {
				public:
					ScalarTrack(ValueTracker *parent, const char *name, const char *vclassName, const char *vname) : 
						Track(parent, VTT_scalar, name, vclassName, vname){}
					virtual ~ScalarTrack(){}

					inline float get(double t){return this->getScalar(t);}
					inline float get(){return this->getScalar(this->parent->mainTimer->getTimems());}
			};

			/**
				Velocity or interpolation track. Between 0 and 1.
			*/
			class VelocityTrack : public Track{
				public:
					VelocityTrack(ValueTracker *parent, const char *name, const char *vclassName, const char *vname): 
						Track(parent, VTT_velocity, name, vclassName, vname){}
					virtual ~VelocityTrack(){}

					inline float get(double t){return this->getVelocity(t);}
					inline float get(){return this->getVelocity(this->parent->mainTimer->getTimems());}
			};

			/**
				Two component vector. Creates two channel.
			*/

			class Vec2Track : public Track{
				public:
					Vec2Track(ValueTracker *parent, const char *name, const char *vclassName, const char *vname): 
					  Track(parent, VTT_vec2, name, vclassName, vname){}
					virtual ~Vec2Track(){}

					inline vec2float get(double t){return this->getVec2(t);}
					inline vec2float get(){return this->getVec2(this->parent->mainTimer->getTimems());}
			};


			/**
				Three component vector. Creates three channel.
			*/
			class Vec3Track : public Track{
				public: 
					Vec3Track(ValueTracker *parent, const char *name, const char *vclassName, const char *vname): 
					  Track(parent, VTT_vec3, name, vclassName, vname){}
					virtual ~Vec3Track(){}

					inline vec3float get(double t){return this->getVec3(t);}
					inline vec3float get(){return this->getVec3(this->parent->mainTimer->getTimems());}
			};

			/**
				Four component vector. Creates four channel.
			*/
			class Vec4Track : public Track{
				public: 
					Vec4Track(ValueTracker *parent, const char *name, const char *vclassName, const char *vname): 
					  Track(parent, VTT_vec4, name, vclassName, vname){}
					virtual ~Vec4Track(){}

					inline vec4float get(double t){return this->getVec4(t);}
					inline vec4float get(){return this->getVec4(this->parent->mainTimer->getTimems());}
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

			inline ValueTracker::Vec2Track* newVec2Track(const char *name, const char *vclassName, const char *vname){
				return new ValueTracker::Vec2Track(this, name, vclassName, vname);}

			inline ValueTracker::Vec3Track* newVec3Track(const char *name, const char *vclassName, const char *vname){
				return new ValueTracker::Vec3Track(this, name, vclassName, vname);}

			inline ValueTracker::Vec4Track* newVec4Track(const char *name, const char *vclassName, const char *vname){
				return new ValueTracker::Vec4Track(this, name, vclassName, vname);}

	};

	// ... 
};


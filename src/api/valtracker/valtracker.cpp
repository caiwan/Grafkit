#include "valtracker.h"
#include "debug/exceptions.h"

#include "math/math.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "rocket/base.h"
#include "rocket/track.h"
#include "rocket/sync.h"

using FWscene::Timer;

#define NoRocketDeviceException() ExceptionEX(200, "No Rocket device")
#define InvalidTrackNameException() ExceptionEX(201, "Invalid track name")

//****************************************************************************************//
// timer callbackek
namespace {
#ifndef SYNC_PLAYER	
		void timer_pause(void *d, int flag)
		{
				Timer *dd = (Timer*)d;  
				dd->togglePause(flag);
		}

		void timer_set_row(void *d, int row)
		{
				Timer *dd = (Timer*)d;
				dd->setRow(row);
		}

		int timer_is_playing(void *d)
		{
				Timer *dd = (Timer*)d;
				return (int)!dd->getPauseFlag();
		}

		struct sync_cb timer_cb = {
				timer_pause,
				timer_set_row,
				timer_is_playing
		};
#endif /*SYNC_PLAYER*/
}

//****************************************************************************************//
FWscene::Timer::Timer(FWplayer::Music *music, long lengthMS, double beatPerMin, int rowPerBeat){
	this->music = music;
	this->length = (double)lengthMS;
	this->bpm = beatPerMin;
	this->rpb = rowPerBeat;

	this->rowLength = (6000.0) / (bpm*rpb);	///@todo timerrel valami el van baszódva
	//this->rowLength = (60.0 * 1000.0) / (bpm*rpb);
	//this->rowLength = 60.0 / (bpm*rpb);

	this->rocket = NULL;
}

FWscene::Timer::~Timer(){
	this->release();
}

// Rocket specific stuff // 
void FWscene::Timer::connect(const char *_ipaddr){
	if (!_ipaddr)
		strcpy(this->ipaddress, "127.0.0.1");
	else
		strncpy(this->ipaddress, _ipaddr, 32);

/* <ROCKET SPECIFIC> */
		rocket = sync_create_device("script/sync");
		if (!rocket) throw new NoRocketDeviceException();

#ifndef SYNC_PLAYER
		//sync_set_callbacks(rocket, &timer_cb, this);
		//if (sync_connect(rocket, this->ipaddress, SYNC_DEFAULT_PORT))
		//        throw new NoRocketDeviceException();

		int row = this->getRowI(); //debugging madorfakor
		if (sync_update(rocket, row, &timer_cb, this))
				if (sync_connect(rocket, this->ipaddress, SYNC_DEFAULT_PORT))
						throw new NoRocketDeviceException();

#endif /*SYNC_PLAYER*/
/* </ROCKET SPECIFIC>*/
}

void FWscene::Timer::update(){
/* <ROCKET SPECIFIC> */
#ifndef SYNC_PLAYER
		int row = this->getRowI(); //debugging madorfakor
		if (sync_update(rocket, row, &timer_cb, this))
				if (sync_connect(rocket, this->ipaddress, SYNC_DEFAULT_PORT))
						throw new NoRocketDeviceException();
#endif /*SYNC_PLAYER*/
/* </ROCKET SPECIFIC>*/
}

void FWscene::Timer::release(){
	if (this->rocket) sync_destroy_device(rocket);
	this->rocket = NULL;
}

void FWscene::Timer::togglePause(int flag){
		bool e = (flag != 0);
		this->music->pause(e);
		this->pauseFlag = e;
}

//****************************************************************************************//
void FWscene::Timer::setRow(int row) const {
		double d = (float)row * (this->rowLength);
		this->music->setTimems(d);
}

double FWscene::Timer::getRowD() const {
		double t = this->getTimems(), d = t / this->rowLength;
		return d; // + 00001; // TODO: scaling
}

int FWscene::Timer::getRowI() const {
		//return (int)floor(this->getRowD());
		float d =this->getRowD();
		int i = (int)(d);
		if (d-(float)i > 0.50) ++i;
		return i;
}

double FWscene::Timer::getRowD(double t) const {
		double d = t / this->rowLength;
		return d; // + 00001; // TODO: scaling
}

int FWscene::Timer::getRowI(double t) const {
		//return (int)floor(this->getRowD());
		float d =this->getRowD(t);
		int i = (int)(d);
		if (d-(float)i > 0.50) ++i;
		return i;
}

int FWscene::Timer::getEnd() const{
#ifndef SYNC_PLAYER
		return 0;       // keep goin forever
#else
	return !this->music->isPlaying();
#endif
}

//****************************************************************************************//
FWscene::ValueTracker::ValueTracker(Timer* timer){
	this->mainTimer = timer;
}

FWscene::ValueTracker::~ValueTracker(){
	//delete this->mainTimer;
}
//****************************************************************************************//

FWscene::ValueTracker::Track::Track(ValueTracker *parent, val_track_e type, const char *name, const char *vclassName, const char *vname){
	this->parent = parent;
	this->type = type;

	char _name[256], _vclass[256], _vname[256];
	int subtracks = 0;
	
	switch (this->type)
	{
	case VTT_bool_switch:
	case VTT_switch:
	case VTT_scalar:
	case VTT_velocity:
		subtracks = 1;
		break;

	case VTT_vec2:
		if (!vname) throw new InvalidTrackNameException();
		subtracks = 2;
		break;

	case VTT_vec3:
		if (!vname) throw new InvalidTrackNameException();
		subtracks = 3;
		break;

	case VTT_vec4:
		if (!vname) throw new InvalidTrackNameException();
		subtracks = 4;
		break;
	}

	for (int i=0; i<0; i++) this->tacks[i] = NULL;

	for (int i=0; i<subtracks; i++){
		if (!vname) sprintf(_name, "%s.%s", name, vclassName);
			else sprintf(_name, "%s.%s.%c", name, vclassName, vname[i]);
			this->tacks[i] = sync_get_track(this->parent->mainTimer->rocket, _name);
	}

}

FWscene::ValueTracker::Track::~Track(){
	
}
///////////////////

int FWscene::ValueTracker::Track::getBool(double t){
	float d = this->parent->mainTimer->getRowD(t);
	float v = sync_get_val(this->tacks[0], d);
	return (v>.75); 
}

int FWscene::ValueTracker::Track::getSwitch(double t){
	float d = this->parent->mainTimer->getRowD(t);
	float v = sync_get_val(this->tacks[0], d);
	return floor(v);
}

float FWscene::ValueTracker::Track::getScalar(double t){
	float d = this->parent->mainTimer->getRowD(t);
	float v = sync_get_val(this->tacks[0], d);
	return v;
}

float FWscene::ValueTracker::Track::getVelocity(double t){
	float d = this->parent->mainTimer->getRowD(t);
	float v = sync_get_val(this->tacks[0], d);
	return FWmath::clampf(v, 0, 1);
}

vec2float FWscene::ValueTracker::Track::getVec2(double t){
	float d = this->parent->mainTimer->getRowD(t);
	vec2float hugyosfoskarika = vec2float(
		sync_get_val(this->tacks[0], d),
		sync_get_val(this->tacks[1], d));
	return hugyosfoskarika;
}

vec3float FWscene::ValueTracker::Track::getVec3(double t){
	float d = this->parent->mainTimer->getRowD(t);
	return vec3float(
		sync_get_val(this->tacks[0], d), 
		sync_get_val(this->tacks[1], d),
		sync_get_val(this->tacks[2], d) );
}

vec4float FWscene::ValueTracker::Track::getVec4(double t){
	float d = this->parent->mainTimer->getRowD(t);
	return vec4float(
		sync_get_val(this->tacks[0], d), 
		sync_get_val(this->tacks[1], d),
		sync_get_val(this->tacks[2], d),
		sync_get_val(this->tacks[2], d) );
}

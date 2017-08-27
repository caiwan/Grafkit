#include "stdafx.h"

#include "valtracker.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#include "rocket/base.h"
#include "rocket/track.h"
#include "rocket/sync.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

//****************************************************************************************//
// timer callbackek
namespace {
#ifndef SYNC_PLAYER	
		void timer_pause(void *d, int flag)
		{
				Timer *dd = (Timer*)d;  
				dd->TogglePause(flag);
		}

		void timer_set_row(void *d, int row)
		{
				Timer *dd = (Timer*)d;
				dd->SetRow(row);
		}

		int timer_is_playing(void *d)
		{
				Timer *dd = (Timer*)d;
				return (int)!dd->GetPauseFlag();
		}

		struct sync_cb timer_cb = {
				timer_pause,
				timer_set_row,
				timer_is_playing
		};
#endif /*SYNC_PLAYER*/
}

//****************************************************************************************//

Grafkit::Timer::Timer() : 
	m_music(),
	m_length(0),
	m_pauseFlag(0),
	m_rocket(nullptr),
	m_rowLength(0),
	m_rowPerBeat(0)
{
	strcpy_s(this->m_ipaddress, 32, "127.0.0.1");
}

Timer::~Timer(){
	this->Shutdown();
}

void Grafkit::Timer::Initialize(Grafkit::MusicResRef music, double lengthMS, double beatPerMin, int rowPerBeat)
{
	m_music = music;
	m_bpm = (beatPerMin), m_length = (lengthMS), m_rowPerBeat = (rowPerBeat); 
	m_rowLength = (60000.0) / (m_bpm*m_rowPerBeat);
}

// Rocket specific stuff // 
void Timer::Connect(const char *_ipaddr){
	if (!_ipaddr)
		strcpy_s(this->m_ipaddress, 32, "127.0.0.1");
	else
		strncpy_s(this->m_ipaddress, 32, _ipaddr, 32);

/* <ROCKET SPECIFIC> */
		m_rocket = sync_create_device("script/sync");
		if (!m_rocket) throw new EX(NoRocketDeviceException);

#ifndef SYNC_PLAYER
		//sync_set_callbacks(rocket, &timer_cb, this);
		//if (sync_connect(rocket, this->ipaddress, SYNC_DEFAULT_PORT))
		//        throw new NoRocketDeviceException();

		int row = this->GetRowI(); //debugging madorfakor
		if (sync_update(m_rocket, row, &timer_cb, this))
				if (sync_connect(m_rocket, this->m_ipaddress, SYNC_DEFAULT_PORT))
						throw new EX(NoRocketDeviceException);

#endif /*SYNC_PLAYER*/
/* </ROCKET SPECIFIC>*/
}

void Timer::Update(){
/* <ROCKET SPECIFIC> */
#ifndef SYNC_PLAYER
		int row = this->GetRowI(); //debugging madorfakor
		if (sync_update(m_rocket, row, &timer_cb, this))
				if (sync_connect(m_rocket, this->m_ipaddress, SYNC_DEFAULT_PORT))
						throw new EX(NoRocketDeviceException);
#endif /*SYNC_PLAYER*/
		this->m_music->Get()->Update();
/* </ROCKET SPECIFIC>*/
}

void Timer::Shutdown(){
	if (this->m_rocket) sync_destroy_device(m_rocket);
	this->m_rocket = NULL;
}

void Timer::TogglePause(int flag){
		bool e = (flag != 0);
		Music()->Pause(e);
		m_pauseFlag = e;
}

//****************************************************************************************//
void Timer::SetRow(int row) const {
		double d = (float)row * (m_rowLength);
		Music()->SetTimems(d);
}

double Timer::GetRowD() const {
		double t = this->GetTimems(), d = t / this->m_rowLength;
		return d; // + 00001; // TODO: scaling
}

int Timer::GetRowI() const {
		//return (int)floor(this->GetRowD());
		double d =this->GetRowD();
		int i = (int)floor(d);
		if (d-(float)i > 0.50) ++i;
		return i;
}

double Timer::GetRowD(double t) const {
		double d = t / this->m_rowLength;
		return d; // + 00001; // TODO: scaling
}

int Timer::GetRowI(double t) const {
		//return (int)floor(this->GetRowD());
		double d =this->GetRowD(t);
		int i = (int)(d);
		if (d-(float)i > 0.50) ++i;
		return i;
}

int Timer::GetEnd() const{
#ifndef SYNC_PLAYER
		return 0;       // keep goin forever
#else
	return !this->music->isPlaying();
#endif
}

//****************************************************************************************//
ValueTracker::ValueTracker(Timer* timer) : m_mainTimer(timer)
{
}

ValueTracker::~ValueTracker(){
	//delete this->m_mainTimer;
}
//****************************************************************************************//

ValueTracker::Track::Track(ValueTracker *parent, val_track_e type, const char *name, const char *vclassName, const char *vname){
	this->m_parent = parent;
	this->m_type = type;

	char _name[256]; // , _vclass[256], _vname[256];
	int subtracks = 0;
	
	switch (this->m_type)
	{
	case VTT_bool_switch:
	case VTT_switch:
	case VTT_scalar:
	case VTT_velocity:
		subtracks = 1;
		break;

	case VTT_Float2:
		if (!vname) throw new EX(InvalidTrackNameException);
		subtracks = 2;
		break;

	case VTT_Float3:
		if (!vname) throw new EX(InvalidTrackNameException);
		subtracks = 3;
		break;

	case VTT_Float4:
		if (!vname) throw new EX(InvalidTrackNameException);
		subtracks = 4;
		break;
	}

	for (int i=0; i<0; i++) this->m_tacks[i] = NULL;

	for (int i=0; i<subtracks; i++){
		if (!vname) 
			sprintf_s(_name, 256, "%s.%s", name, vclassName);
		else 
			sprintf_s(_name, 256, "%s.%s.%c", name, vclassName, vname[i]);
		
		this->m_tacks[i] = sync_get_track(this->m_parent->m_mainTimer->m_rocket, _name);
	}

}

ValueTracker::Track::~Track(){
	
}
///////////////////

int ValueTracker::Track::GetBool(double t){
	double d = this->m_parent->m_mainTimer->GetRowD(t);
	double v = sync_get_val(this->m_tacks[0], d);
	return (v>.5); 
}

int ValueTracker::Track::GetSwitch(double t){
	double d = this->m_parent->m_mainTimer->GetRowD(t);
	double v = sync_get_val(this->m_tacks[0], d);
	return (int)floor(v);
}

float ValueTracker::Track::GetScalar(double t){
	double d = this->m_parent->m_mainTimer->GetRowD(t);
	double v = sync_get_val(this->m_tacks[0], d);
	return (float)v;
}

float ValueTracker::Track::GetVelocity(double t){
	double d = this->m_parent->m_mainTimer->GetRowD(t);
	double v = sync_get_val(this->m_tacks[0], d);
	return (float)v;
}

float2 ValueTracker::Track::GetFloat2(double t){
	double d = this->m_parent->m_mainTimer->GetRowD(t);
	float2 res;
	res.x = (float)sync_get_val(this->m_tacks[0], d),
	res.y = (float)sync_get_val(this->m_tacks[1], d);
	return res;
}

float3 ValueTracker::Track::GetFloat3(double t){
	double d = this->m_parent->m_mainTimer->GetRowD(t);
	float3 res;
	res.x = (float)sync_get_val(this->m_tacks[0], d),
	res.y = (float)sync_get_val(this->m_tacks[1], d),
	res.z = (float)sync_get_val(this->m_tacks[2], d);
	return res;
}

float4 ValueTracker::Track::GetFloat4(double t){
	double d = this->m_parent->m_mainTimer->GetRowD(t);
	float4 res;
	res.x= (float)sync_get_val(this->m_tacks[0], d), 
	res.y= (float)sync_get_val(this->m_tacks[1], d),
	res.z= (float)sync_get_val(this->m_tacks[2], d),
	res.w= (float)sync_get_val(this->m_tacks[2], d);
	return res;
}

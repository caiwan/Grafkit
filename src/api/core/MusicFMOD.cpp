#include "MusicFMOD.h"

#include "../debug/exceptions.h"
#include "../debug/test.h"
#include "../GKSDK_config.h"

///@todo FFTprovider::FMOD_RESULT F_API getSpectrum (float *spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
///@todo +ami a qfmodex-ben van

namespace {
}

#define FMOD_ERRCHECK(x) \
	if ((x) != FMOD_OK) \
		throw new ExceptionEX((int)x, "FMOD error");

void FWplayer::MusicFMOD::init(const void* data, unsigned int len){
	FMOD_RESULT result;
	FMOD_CREATESOUNDEXINFO exinfo;
	
	memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
	exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	
	exinfo.length = len;

	System_Create(&this->system);
	result = this->system->init(1, FMOD_INIT_NORMAL, 0); FMOD_ERRCHECK(result);
	
	///@todo ha nem szerkesztoben van hasznalva, akkor create strem, kulonben sound < makrokkal elintezni
	//result = this->system->createStream((char*)data, FMOD_OPENMEMORY | FMOD_HARDWARE, &exinfo, &this->sound); FMOD_ERRCHECK(result);
	result = this->system->createSound((char*)data, FMOD_OPENMEMORY | FMOD_SOFTWARE | FMOD_CREATESAMPLE | FMOD_UNIQUE | FMOD_LOOP_OFF | FMOD_2D | FMOD_ACCURATETIME, &exinfo, &this->sound); FMOD_ERRCHECK(result);

	init();
}

void FWplayer::MusicFMOD::init(){
	this->sound->getLength(&this->length, FMOD_TIMEUNIT_PCM);
	
	/*
	int samplerate = 0;
	FMOD_SOUND_FORMAT format;
	int numoutputchannels = 0 ;
	int maxinputchannels = 0;
	FMOD_DSP_RESAMPLER resamplemethod;
	*/
	int bits = 0;
	// ez lehet, hogy nem fogja szeretni ezt a sok nullt:
	this->system->getSoftwareFormat(&this->samplePerSec, NULL, NULL, NULL, NULL, &bits);
	//this->system->getSoftwareFormat(&this->samplePerSec, &format, &numoutputchannels, &maxinputchannels, &resamplemethod, &bits);

	DEBUG_DO(FWcore::writeLog2("Music loaded. Length = %d samples; %.3f sec ", this->length, (float)this->length/(float)this->samplePerSec));
}

FWplayer::MusicFMOD::MusicFMOD(const char* filename) : 
	Music(), AudioVisPRovider(),

	res(NULL),
	sound(NULL),
	system(NULL),
	channel(NULL)
{
	FMOD_RESULT result ;
	
	System_Create(&this->system);
	result = system->init(1, FMOD_INIT_NORMAL, 0);

	///@todo ha nem szerkesztoben van hasznalva, akkor create strem, kulonben sound < makrokkal elintezni
	result = this->system->createSound(filename, FMOD_SOFTWARE | FMOD_CREATESAMPLE | FMOD_UNIQUE | FMOD_LOOP_OFF | FMOD_2D | FMOD_ACCURATETIME, 0, &this->sound);
	//result = this->system->createStream(filename, FMOD_HARDWARE | FMOD_CREATESAMPLE, 0, &this->sound);
	
	///@todo check 'result'

	init();
}

FWplayer::MusicFMOD::MusicFMOD(const void* data, unsigned int len) : 
	Music(), // FFTProvider(),
	
	res(NULL),
	sound(NULL),
	system(NULL),
	channel(NULL)
{
	init(data, len);
}

FWplayer::MusicFMOD::MusicFMOD(Resource *res) : 
	Music(), // FFTProvider(),

	res(NULL),
	sound(NULL),
	system(NULL),
	channel(NULL)
{
	if (!res) throw NullPointerException();
	
	res->load();
	init(res->getData(), (unsigned int)res->getLength());
}

FWplayer::MusicFMOD::~MusicFMOD(void)
{
	this->stop();
	//@todo: toroljon le minden mas resource-t is
	if (this->res) delete res;
}

void FWplayer::MusicFMOD::play()
{
	FMOD_RESULT result = this->system->playSound(FMOD_CHANNEL_FREE, this->sound, false, &this->channel);
}

void FWplayer::MusicFMOD::stop()
{
	FMOD_RESULT result = this->channel->stop();
}

void FWplayer::MusicFMOD::pause(bool e)
{
	bool b = 0;
	this->channel->isPlaying(&b);
	
	if (!b && !e){
		this->play();	
	}

	FMOD_RESULT result = this->channel->setPaused(e);
}

unsigned int FWplayer::MusicFMOD::getTimeSample()
{
	unsigned int t = 0;
	this->channel->getPosition(&t, FMOD_TIMEUNIT_PCM);
	return t;
}

void FWplayer::MusicFMOD::setTimeSample(unsigned int t)
{
	this->channel->setPosition(t, FMOD_TIMEUNIT_PCM);
}

void FWplayer::MusicFMOD::setLoop(bool e)
{
	if (e) this->channel->setMode(FMOD_LOOP_NORMAL);
	else this->channel->setMode(FMOD_LOOP_OFF);
}

int FWplayer::MusicFMOD::isPlaying()
{
	bool b = 0, p = 0;
	this->channel->isPlaying(&b); 
	this->channel->getPaused(&p);
	return b && !p;
}
#include "MusicFmodLoader.h"

#include "../core/Music.h"
#include "fmodex/fmod.hpp"

using namespace Grafkit;
using namespace FWdebugExceptions;

/// ====================================================================================================================================================
/// Fmod music class implementation
/// ====================================================================================================================================================
namespace {

	class MusicFMOD : public Music {

	public:
		MusicFMOD();
		virtual ~MusicFMOD(void);

		void Initialize(IAssetRef asset);

		virtual void Play();
		virtual void Stop();
		virtual void Pause(bool e);

		virtual unsigned int GetTimeSample();
		virtual void SetTimeSample(unsigned int t);
		virtual void SetLoop(bool e);
		virtual int IsPlaying();

	protected:
		FMOD::System *m_system;
		FMOD::Sound *m_sound;
		FMOD::Channel *m_channel;

		Grafkit::IAssetRef asset;

	};

#define FMOD_ERRCHECK(x) \
	if ((x) != FMOD_OK) \
	throw EX_DETAILS(MusicLoadException, "FMOD ERROR"); // new ExceptionEX((int)x, "FMOD error");

	MusicFMOD::MusicFMOD() : Music(),
		m_system(nullptr),
		m_sound(nullptr),
		m_channel(nullptr)
	{
	}

	MusicFMOD::~MusicFMOD(void)
	{
		Stop();
		m_system->close();
		// Itt elvileg valamikor meg kellene szunnie a tobbi handlernek valamikor
	}

	void MusicFMOD::Initialize(IAssetRef asset)
	{
		const void* data = asset->GetData();
		size_t len = asset->GetSize();
		
		FMOD_RESULT result;
		FMOD_CREATESOUNDEXINFO exinfo;
		
		memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
		exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		
		exinfo.length = len;
		
		FMOD::System_Create(&m_system);
		result = m_system->init(1, FMOD_INIT_NORMAL, 0); FMOD_ERRCHECK(result);
		
		///@todo ha nem szerkesztoben van hasznalva, akkor create strem, kulonben sound < makrokkal elintezni
		//result = this->system->createStream((char*)data, FMOD_OPENMEMORY | FMOD_HARDWARE, &exinfo, &this->sound); FMOD_ERRCHECK(result);
		result = m_system->createSound((char*)data, FMOD_OPENMEMORY | FMOD_SOFTWARE | FMOD_CREATESAMPLE | FMOD_UNIQUE | FMOD_LOOP_OFF | FMOD_2D | FMOD_ACCURATETIME, &exinfo, &m_sound); FMOD_ERRCHECK(result);

		m_sound->getLength(&m_length, FMOD_TIMEUNIT_PCM);

		/*
		int samplerate = 0;
		FMOD_SOUND_FORMAT format;
		int numoutputchannels = 0 ;
		int maxinputchannels = 0;
		FMOD_DSP_RESAMPLER resamplemethod;
		*/

		int bits = 0;
		// ez lehet, hogy nem fogja szeretni ezt a sok nullt:
		m_system->getSoftwareFormat(&m_samplePerSec, NULL, NULL, NULL, NULL, &bits);
		//this->system->getSoftwareFormat(&this->samplePerSec, &format, &numoutputchannels, &maxinputchannels, &resamplemethod, &bits);

		LOGGER(Log::Logger().Info("Music loaded. Length = %d samples; %.3f sec ", m_length, (float)m_length / (float)m_samplePerSec));
	}

	void MusicFMOD::Play()
	{
		FMOD_RESULT result = m_system->playSound(FMOD_CHANNEL_FREE, m_sound, false, &m_channel);
		FMOD_ERRCHECK(result);
	}

	void MusicFMOD::Stop()
	{
		FMOD_RESULT result = m_channel->stop();
		FMOD_ERRCHECK(result);
	}

	void MusicFMOD::Pause(bool e)
	{
		bool b = 0;
		m_channel->isPlaying(&b);
		
		if (!b && !e) {
			Play();
		}
		
		FMOD_RESULT result = m_channel->setPaused(e);
	}

	unsigned int MusicFMOD::GetTimeSample()
	{
		unsigned int t = 0;
		m_channel->getPosition(&t, FMOD_TIMEUNIT_PCM);
		return t;
	}

	void MusicFMOD::SetTimeSample(unsigned int t)
	{
		m_channel->setPosition(t, FMOD_TIMEUNIT_PCM);
	}

	void MusicFMOD::SetLoop(bool e)
	{
		if (e) 
			m_channel->setMode(FMOD_LOOP_NORMAL);
		else 
			m_channel->setMode(FMOD_LOOP_OFF);
	}

	int MusicFMOD::IsPlaying()
	{
		bool b = 0, p = 0;
		m_channel->isPlaying(&b);
		m_channel->getPaused(&p);
		return b && !p;
	}

}

/// ====================================================================================================================================================
/// Factory class implementation
/// ====================================================================================================================================================
Grafkit::MusicFmodLoader::MusicFmodLoader(std::string source_name) : Grafkit::IResourceBuilder(source_name, source_name)
{
}

Grafkit::MusicFmodLoader::~MusicFmodLoader()
{
}

void Grafkit::MusicFmodLoader::Load(IResourceManager * const & resman, IResource * source)
{
}

IResource * Grafkit::MusicFmodLoader::NewResource()
{
	return nullptr; //folytkov
}

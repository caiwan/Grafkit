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
	}

	void MusicFMOD::Initialize(IAssetRef asset)
	{
		/*const void* data, unsigned int len*/
		
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
	}

	void MusicFMOD::Stop()
	{
	}

	void MusicFMOD::Pause(bool e)
	{
	}

	unsigned int MusicFMOD::GetTimeSample()
	{
		return 0;
	}

	void MusicFMOD::SetTimeSample(unsigned int t)
	{
	}

	void MusicFMOD::SetLoop(bool e)
	{
	}

	int MusicFMOD::IsPlaying()
	{
		return 0;
	}


//
//	void MusicFMOD::init(const void* data, unsigned int len) {

//		init();
//	}
//
//	void MusicFMOD::init() {

//	}
//
//	MusicFMOD::MusicFMOD(const char* filename) :
//		Music(), AudioVisPRovider(),
//
//		res(NULL),
//		sound(NULL),
//		system(NULL),
//		channel(NULL)
//	{
//		FMOD_RESULT result;
//
//		System_Create(&this->system);
//		result = system->init(1, FMOD_INIT_NORMAL, 0);
//
//		///@todo ha nem szerkesztoben van hasznalva, akkor create strem, kulonben sound < makrokkal elintezni
//		result = this->system->createSound(filename, FMOD_SOFTWARE | FMOD_CREATESAMPLE | FMOD_UNIQUE | FMOD_LOOP_OFF | FMOD_2D | FMOD_ACCURATETIME, 0, &this->sound);
//		//result = this->system->createStream(filename, FMOD_HARDWARE | FMOD_CREATESAMPLE, 0, &this->sound);
//
//		///@todo check 'result'
//
//		init();
//	}
//
//	MusicFMOD::MusicFMOD(const void* data, unsigned int len) :
//		Music(), // FFTProvider(),
//
//		res(NULL),
//		sound(NULL),
//		system(NULL),
//		channel(NULL)
//	{
//		init(data, len);
//	}
//
//	MusicFMOD::MusicFMOD(Resource *res) :
//		Music(), // FFTProvider(),
//
//		res(NULL),
//		sound(NULL),
//		system(NULL),
//		channel(NULL)
//	{
//		if (!res) throw NullPointerException();
//
//		res->load();
//		init(res->getData(), (unsigned int)res->getLength());
//	}
//
//	MusicFMOD::~MusicFMOD(void)
//	{
//		this->stop();
//		//@todo: toroljon le minden mas resource-t is
//		if (this->res) delete res;
//	}
//
//	void MusicFMOD::play()
//	{
//		FMOD_RESULT result = this->system->playSound(FMOD_CHANNEL_FREE, this->sound, false, &this->channel);
//	}
//
//	void MusicFMOD::stop()
//	{
//		FMOD_RESULT result = this->channel->stop();
//	}
//
//	void MusicFMOD::pause(bool e)
//	{
//		bool b = 0;
//		this->channel->isPlaying(&b);
//
//		if (!b && !e) {
//			this->play();
//		}
//
//		FMOD_RESULT result = this->channel->setPaused(e);
//	}
//
//	unsigned int MusicFMOD::getTimeSample()
//	{
//		unsigned int t = 0;
//		this->channel->getPosition(&t, FMOD_TIMEUNIT_PCM);
//		return t;
//	}
//
//	void MusicFMOD::setTimeSample(unsigned int t)
//	{
//		this->channel->setPosition(t, FMOD_TIMEUNIT_PCM);
//	}
//
//	void MusicFMOD::setLoop(bool e)
//	{
//		if (e) this->channel->setMode(FMOD_LOOP_NORMAL);
//		else this->channel->setMode(FMOD_LOOP_OFF);
//	}
//
//	int MusicFMOD::isPlaying()
//	{
//		bool b = 0, p = 0;
//		this->channel->isPlaying(&b);
//		this->channel->getPaused(&p);
//		return b && !p;
//	}
//
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
	return nullptr;
}

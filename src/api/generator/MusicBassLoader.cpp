#include "MusicBassLoader.h"

#include "../core/Music.h"
#include "bass.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

/// ====================================================================================================================================================
/// Bass music class implementation
/// ====================================================================================================================================================
namespace {

	class MusicBass : public Music {

	public:
		MusicBass();
		virtual ~MusicBass(void);

		void Initialize(IAssetRef asset);
		void Shutdown();

		virtual void Play();
		virtual void Stop();
		virtual void Pause(bool e);

		virtual unsigned int GetTimeSample();
		virtual void SetTimeSample(unsigned int t);
		virtual void SetLoop(bool e);
		virtual int IsPlaying();

	protected:
		HSTREAM m_stream;
		int m_is_playing;

		Grafkit::IAssetRef asset;

	};

#define FMOD_ERRCHECK(x) \
	if ((x) != FMOD_OK) \
	throw EX_DETAILS(MusicLoadException, "FMOD ERROR"); // new ExceptionEX((int)x, "FMOD error");

	MusicBass::MusicBass() : Music(),
		m_stream(0),
		m_is_playing(0),
		asset()
	{
		//int res = BASS_Init(-1, 44100, 0, 0, 0);

		//if (!res) {
		//	// int errcode = BASS_ErrorGetCode();
		//	throw EX(MusicDeviceInitException);
		//}

		//m_samplePerSec = 44100;
	}

	MusicBass::~MusicBass(void)
	{
		Shutdown();
		//m_system->close();
		// Itt elvileg valamikor meg kellene szunnie a tobbi handlernek valamikor
	}

	void MusicBass::Initialize(IAssetRef asset)
	{
		//const void* data = asset->GetData();
		//size_t len = asset->GetSize();

		//m_stream = BASS_StreamCreateFile(TRUE, data, 0, len,
		//	BASS_STREAM_PRESCAN |
		//	0
		//);

		//if (!m_stream) {
		//	// int errcode = BASS_ErrorGetCode();
		//	throw EX(MusicDeviceInitException);
		//}

		//BASS_ChannelGetLength(m_stream, BASS_POS_BYTE);

	}

	void MusicBass::Shutdown()
	{
		//if(IsPlaying()) 
		//	Stop();
	}

	void MusicBass::Play()
	{
		//BASS_ChannelPlay(m_stream, 1);
		//m_is_playing = 1;
	}

	void MusicBass::Stop()
	{
		//BASS_ChannelStop(m_stream);
		//m_is_playing = 0;
	}

	void MusicBass::Pause(bool e)
	{
	
		//if (!m_is_playing && !e) {
		//	BASS_ChannelPlay(m_stream, 0);
		//	m_is_playing = 1;
		//}
		//else {
		//	BASS_Pause();
		//	m_is_playing = 0;
		//}
	}

	unsigned int MusicBass::GetTimeSample()
	{
		//QWORD pos = BASS_ChannelGetPosition(m_stream, BASS_POS_BYTE);
		//return pos;
		return 0;
	}

	void MusicBass::SetTimeSample(unsigned int t)
	{
		//m_channel->setPosition(t, FMOD_TIMEUNIT_PCM);
	}

	void MusicBass::SetLoop(bool e)
	{
		// nincsilyen
	}

	int MusicBass::IsPlaying()
	{
		/*bool b = 0, p = 0;
		m_channel->isPlaying(&b);
		m_channel->getPaused(&p);
		return b && !p;

		BASS_CHANNELINFO info;
		BASS_ChannelGetInfo(m_stream, &info);*/

		return 0;
	}

}

/// ====================================================================================================================================================
/// Factory class implementation
/// ====================================================================================================================================================
Grafkit::MusicBassLoader::MusicBassLoader(std::string source_name) : Grafkit::IResourceBuilder(source_name, source_name)
{
}

Grafkit::MusicBassLoader::~MusicBassLoader()
{
}

void Grafkit::MusicBassLoader::Load(IResourceManager * const & resman, IResource * source)
{
	MusicResRef dest = dynamic_cast<MusicRes*>(source);
	if (dest.Invalid()) {
		throw EX(NullPointerException);
	}

	Music* music = new MusicBass();

	IAssetRef asset = this->GetSourceAsset(resman);
	music->Initialize(asset);

	dest->AssingnRef(music);
}

IResource * Grafkit::MusicBassLoader::NewResource()
{
	return new MusicRes();
}

#include "stdafx.h"

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
		virtual void Update();

		virtual unsigned long GetTimeSample();
		virtual void SetTimeSample(unsigned long t);
		virtual void SetLoop(bool e);
		virtual int IsPlaying();

		virtual bool GetFFT(float* ptr, int segcount);

	protected:
		HSTREAM m_stream;

		Grafkit::IAssetRef m_asset;

	};

	MusicBass::MusicBass() : Music(),
		m_stream(0),
		m_asset()
	{
	}

	MusicBass::~MusicBass(void)
	{
		Shutdown();
		//m_system->close();
		// Itt elvileg valamikor meg kellene szunnie a tobbi handlernek valamikor
	}

	void MusicBass::Initialize(IAssetRef asset)
	{
		m_asset = asset;
		const void* data = m_asset->GetData();
		size_t data_size = m_asset->GetSize();

		int res = BASS_Init(-1, 44100, 0, 0, 0);

		if (!res) {
			// int errcode = BASS_ErrorGetCode();
			throw EX(MusicDeviceInitException);
		}

		m_stream = BASS_StreamCreateFile(TRUE, data, 0, data_size,
			BASS_STREAM_PRESCAN |
			0
		);

		if (!m_stream) {
			//int errcode = BASS_ErrorGetCode();
			throw EX(MusicDeviceInitException);
		}

		BASS_Start();

		BASS_CHANNELINFO info;
		BASS_ChannelGetInfo(m_stream, &info);

		m_length = BASS_ChannelGetLength(m_stream, BASS_POS_BYTE);
		
		// length in byte -> samples 
		int sstride = 2; 
		if (info.flags & BASS_SAMPLE_8BITS) sstride = 1; else if (info.flags & BASS_SAMPLE_FLOAT) sstride = 4;

		m_samplePerSec = info.freq * sstride * info.chans;
	
		
	}

	void MusicBass::Shutdown()
	{
		BASS_ChannelStop(m_stream);
		BASS_StreamFree(m_stream);
		BASS_Stop();
		BASS_Free();
	}

	void MusicBass::Play()
	{
		BASS_ChannelPlay(m_stream, 0);
	}

	void MusicBass::Stop()
	{
		BASS_ChannelStop(m_stream);
	}

	void MusicBass::Pause(bool e)
	{
		bool b = IsPlaying();
		if (!b && !e) {
			BASS_ChannelPlay(m_stream, 0);
		}
		else {
			BASS_ChannelPause(m_stream);
		}
	}

	void MusicBass::Update()
	{
		BASS_Update(0);
	}

	unsigned long MusicBass::GetTimeSample()
	{
		QWORD pos = BASS_ChannelGetPosition(m_stream, BASS_POS_BYTE);
		return pos;
	}

	void MusicBass::SetTimeSample(unsigned long t)
	{
		BASS_ChannelSetPosition(m_stream, t, BASS_POS_BYTE);
	}

	void MusicBass::SetLoop(bool e)
	{
		BASS_ChannelFlags(m_stream, BASS_SAMPLE_LOOP * e, BASS_SAMPLE_LOOP);
	}

	int MusicBass::IsPlaying()
	{
		return BASS_ChannelIsActive(m_stream) == BASS_ACTIVE_PLAYING;
	}

	// taken from Gargaj 
	// https://github.com/Gargaj/Bonzomatic/blob/e1aa90ba5c47a6aa61bcf754a634d09e2ef23f81/src/platform_common/FFT.cpp#L28
	bool MusicBass::GetFFT(float* ptr, int segcount) 
	{
		unsigned int len = 0;

		if (!IsPlaying())
			return false;

		switch (segcount * 2) // for 256 fft, only 128 values will contain DC in our case
		{
		case 256:
			len = BASS_DATA_FFT256;
			break;
		case 512:
			len = BASS_DATA_FFT512;
			break;
		case 1024:
			len = BASS_DATA_FFT1024;
			break;
		case 2048:
			len = BASS_DATA_FFT2048;
			break;
		case 4096:
			len = BASS_DATA_FFT4096;
			break;
		case 8192:
			len = BASS_DATA_FFT8192;
			break;
		case 16384:
			len = BASS_DATA_FFT16384;
			break;
		default:
			return false;
		}

		const int numBytes = BASS_ChannelGetData(m_stream, ptr, len | BASS_DATA_FFT_REMOVEDC);
		if (numBytes <= 0)
			return false;

		return true;

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

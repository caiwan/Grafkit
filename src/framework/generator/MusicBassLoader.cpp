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
		virtual void Pause(int e);
		virtual void Update();
		virtual void ToggleMute();

		virtual uint64_t GetSampleCount() { return m_length; }
		virtual uint64_t GetSampleCountPerSec() { return m_samplePerSec; }

		virtual uint64_t GetTimeSample();
		virtual void SetTimeSample(uint64_t t);
		virtual void SetLoop(int e);
		virtual int IsPlaying();

		virtual bool GetFFT(float* ptr, int segcount);
		virtual bool GetWaveform(float *& ptr, size_t &length, size_t &channelCount, size_t& samplePerSec);

	protected:
		HSTREAM m_stream;
		Grafkit::IAssetRef m_asset;

	private:
		bool m_isMute;

		uint64_t m_length;
		uint64_t m_samplePerSec;

	};

	MusicBass::MusicBass() : Music(),
		m_stream(0),
		m_asset(), m_isMute(false)
	{
	}

	MusicBass::~MusicBass(void)
	{
		Shutdown();
	}

	void MusicBass::Initialize(IAssetRef asset)
	{
		m_asset = asset;
		const void* data = m_asset->GetData();
		size_t data_size = m_asset->GetSize();

		int res = BASS_Init(-1, 44100, 0, 0, 0);

		bool isNosound = false;
		if (!res) {

			// ignore sound if no sound device detected or installed
			int errcode = BASS_ErrorGetCode();
			if (errcode == 23) {
				res = BASS_Init(0, 44100, 0, 0, 0);
				isNosound = true;
			}
			if (!res)
				throw new EX(MusicDeviceInitException);
		}

		m_stream = BASS_StreamCreateFile(TRUE, data, 0, data_size,
			BASS_STREAM_PRESCAN | (isNosound ? BASS_DEVICE_NOSPEAKER : 0) |
			0
		);

		if (!m_stream) {
			int errcode = BASS_ErrorGetCode();
			throw new EX(MusicDeviceInitException);
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

	void MusicBass::Pause(int e)
	{
		int b = IsPlaying();
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

	void MusicBass::ToggleMute()
	{
		if (!m_isMute)
			BASS_ChannelSetAttribute(m_stream, BASS_ATTRIB_VOL, 0.0f);
		else
			BASS_ChannelSetAttribute(m_stream, BASS_ATTRIB_VOL, 1.0f);
		m_isMute = !m_isMute;
	}

	uint64_t MusicBass::GetTimeSample()
	{
		QWORD pos = BASS_ChannelGetPosition(m_stream, BASS_POS_BYTE);
		return pos;
	}

	void MusicBass::SetTimeSample(uint64_t t)
	{
		BASS_ChannelSetPosition(m_stream, t, BASS_POS_BYTE);
	}

	void MusicBass::SetLoop(int e)
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

	bool MusicBass::GetWaveform(float *& ptr, size_t &length, size_t &channelCount, size_t &samplePerSec)
	{
		length = 0;
		ptr = nullptr;
		samplePerSec = 0;
		BASS_CHANNELINFO channelInfo;
		if (!BASS_ChannelGetInfo(m_stream, &channelInfo))
			return false;

		length = m_length;
		channelCount = channelInfo.chans;
		samplePerSec = m_samplePerSec;

		ptr = new float[length * channelCount];

		const int numBytes = BASS_ChannelGetData(m_stream, ptr, BASS_DATA_FLOAT);

		if (numBytes == -1)
		{
			delete ptr;
			length = 0;
			ptr = nullptr;
			samplePerSec = 0;
			return false;
		}

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
		throw new EX(NullPointerException);
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

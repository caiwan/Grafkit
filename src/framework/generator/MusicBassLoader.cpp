#include "stdafx.h"

#include "MusicBassLoader.h"

#include "../core/Music.h"
#include "bass.h"

#include "utils/logger.h"

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

		virtual void GetFFT(float* ptr, int segcount);
		virtual void GetWaveform(float *& ptr, size_t &length, size_t &channelCount, size_t& samplePerSec);

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
	    const size_t dataSize = m_asset->GetSize();

		int res = BASS_Init(-1, 44100, 0, 0, 0);

		bool isNosound = false;
		if (!res) {

			// ignore sound if no sound device detected or installed
		    const int errorCode = BASS_ErrorGetCode();
			if (errorCode == 23) {
				res = BASS_Init(0, 44100, 0, nullptr, nullptr);
				isNosound = true;
			}
			if (!res)
				throw new EX(MusicDeviceInitException);
		}

		m_stream = BASS_StreamCreateFile(TRUE, data, 0, dataSize,
			BASS_STREAM_PRESCAN | (isNosound ? BASS_DEVICE_NOSPEAKER : 0) |
			0
		);

		if (!m_stream) {
			int errcode = BASS_ErrorGetCode();
			LOGGER(Log::Logger().Error("Failed to laod music. BASS Error code: %d", errcode));
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
	    const int b = IsPlaying();
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
	    const QWORD position = BASS_ChannelGetPosition(m_stream, BASS_POS_BYTE);
		return position;
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
	void MusicBass::GetFFT(float* ptr, int segcount)
	{
		unsigned int len = 0;

		if (!IsPlaying())
			return;

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
			return;
		}

		/*const int numBytes =*/ BASS_ChannelGetData(m_stream, ptr, len | BASS_DATA_FFT_REMOVEDC);

	}

	void MusicBass::GetWaveform(float *& ptr, size_t &length, size_t &channelCount, size_t &samplePerSec)
	{
		length = 0;
		ptr = nullptr;
		samplePerSec = 0;
		BASS_CHANNELINFO channelInfo;
		if (!BASS_ChannelGetInfo(m_stream, &channelInfo))
			return;

		length = m_length;
		channelCount = channelInfo.chans;
		samplePerSec = m_samplePerSec;

	    const size_t buflen = length * channelCount;
		ptr = new float[buflen];

		memset(ptr, 0, buflen * 4);

		// TODO: ez nem jo
		//const int numBytes = BASS_ChannelGetData(m_stream, ptr, BASS_DATA_FLOAT);
		const int numBytes = BASS_ChannelGetData(m_stream, ptr, buflen * 4 | BASS_DATA_FLOAT);
		//BASS_RecordFree

		if (numBytes == -1)
		{
			LOGGER(Log::Logger().Error("Could not read waveform data. BASS ERROR: %d", BASS_ErrorGetCode()));

			delete[] ptr;
			length = 0;
			ptr = nullptr;
			samplePerSec = 0;
		}
	}
}



/// ====================================================================================================================================================
/// Factory class implementation
/// ====================================================================================================================================================
Grafkit::MusicBassLoader::MusicBassLoader(const std::string& sourceName) : Grafkit::IResourceBuilder(sourceName, sourceName)
{
}

Grafkit::MusicBassLoader::~MusicBassLoader()
= default;

void Grafkit::MusicBassLoader::Load(IResourceManager * const & resman, IResource * source)
{
	MusicResRef dest = dynamic_cast<MusicRes*>(source);
	if (dest.Invalid()) {
		throw new EX(NullPointerException);
	}

	Music* music = new MusicBass();

    const IAssetRef asset = this->GetSourceAsset(resman);
	music->Initialize(asset);

	dest->AssingnRef(music);
}

IResource * Grafkit::MusicBassLoader::NewResource()
{
	return new MusicRes();
}

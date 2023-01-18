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

        void Initialize(IAssetRef asset) override;
        void Shutdown() override;

        void Play() override;
        void Stop() override;
        void Pause(int e) override;
        void Update() override;
        void ToggleMute() override;

        uint64_t GetSampleCount() override { return m_length; }
        uint64_t GetSampleCountPerSec() override { return m_samplePerSec; }

        uint64_t GetTimeSample() override;
        void SetTimeSample(uint64_t t) override;
        void SetLoop(int e) override;
        int IsPlaying() override;

        void GetFFT(float* ptr, int segcount) override;
        void GetWaveform(float *& ptr, size_t &length, size_t &channelCount, size_t& samplePerSec) override;

    protected:
        //HSTREAM m_sample;
        IAssetRef m_asset;
        HSAMPLE m_sample;
        HCHANNEL m_channel;

    private:
        bool m_isMute;

        uint64_t m_length;
        uint64_t m_samplePerSec;
        uint64_t m_bytesPerSample;

    };

    MusicBass::MusicBass() : Music()
        , //m_sample(0),
        m_asset()
        , m_sample(0)
        , m_isMute(false) {
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

        // TODO: Put init to static or something

        int res = BASS_Init(-1, 44100, 0, nullptr, nullptr);

        bool isNosound = false;
        if (!res) {

            // ignore sound if no sound device detected or installed
            const int errorCode = BASS_ErrorGetCode();

            // hack something around error codes
            if (errorCode != BASS_ERROR_ALREADY)
            {
                if (errorCode == BASS_ERROR_DEVICE) {
                    res = BASS_Init(0, 44100, 0, nullptr, nullptr);
                    isNosound = true;
                }
                if (!res)
                {
                    LOGGER(Log::Logger().Error("BASS Error code %d", errorCode));
                    THROW_EX(MusicDeviceInitException);
                }
            }
        }

        m_sample = BASS_SampleLoad(true, data, 0, dataSize, 1, BASS_STREAM_PRESCAN | (isNosound ? BASS_DEVICE_NOSPEAKER : 0) | 0);

        //m_sample = BASS_StreamCreateFile(TRUE, data, 0, dataSize,
        //	BASS_STREAM_PRESCAN | (isNosound ? BASS_DEVICE_NOSPEAKER : 0) |
        //	0
        //);

        if (!m_sample) {
            int errcode = BASS_ErrorGetCode();
            LOGGER(Log::Logger().Error("Failed to laod music. BASS Error code: %d", errcode));
            THROW_EX(MusicDeviceInitException);
        }

        BASS_Start();

        m_channel = BASS_SampleGetChannel(m_sample, false);

        BASS_CHANNELINFO info;
        BASS_ChannelGetInfo(m_channel, &info);

        m_length = BASS_ChannelGetLength(m_channel, BASS_POS_BYTE);

        // length in byte -> samples 
        int sstride = 2;
        if (info.flags & BASS_SAMPLE_8BITS) sstride = 1; else if (info.flags & BASS_SAMPLE_FLOAT) sstride = 4;

        m_samplePerSec = info.freq;
        m_bytesPerSample = sstride * info.chans;

    }

    void MusicBass::Shutdown()
    {
        BASS_ChannelStop(m_channel);
        BASS_SampleFree(m_sample);
        BASS_Stop();
        BASS_Free();
    }

    void MusicBass::Play()
    {
        BASS_ChannelPlay(m_channel, 1);
    }

    void MusicBass::Stop()
    {
        BASS_ChannelSetPosition(m_channel, 0, BASS_POS_BYTE);
        BASS_ChannelPause(m_channel);
    }

    void MusicBass::Pause(int e)
    {
        const int b = IsPlaying();
        if (!b && !e) {
            BASS_ChannelPlay(m_channel, 0);
        }
        else {
            BASS_ChannelPause(m_channel);
        }
    }

    void MusicBass::Update()
    {
        BASS_Update(0);
    }

    void MusicBass::ToggleMute()
    {
        if (!m_isMute)
            BASS_ChannelSetAttribute(m_channel, BASS_ATTRIB_VOL, 0.0f);
        else
            BASS_ChannelSetAttribute(m_channel, BASS_ATTRIB_VOL, 1.0f);
        m_isMute = !m_isMute;
    }

    uint64_t MusicBass::GetTimeSample()
    {
        QWORD position = BASS_ChannelGetPosition(m_channel, BASS_POS_BYTE);
        position /= m_bytesPerSample;
        return position;
    }

    void MusicBass::SetTimeSample(uint64_t t)
    {
        BASS_ChannelSetPosition(m_channel, t * m_bytesPerSample, BASS_POS_BYTE);
    }

    void MusicBass::SetLoop(int e)
    {
        BASS_ChannelFlags(m_channel, BASS_SAMPLE_LOOP * e, BASS_SAMPLE_LOOP);
    }

    int MusicBass::IsPlaying()
    {
        return BASS_ChannelIsActive(m_channel) == BASS_ACTIVE_PLAYING;
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

        /*const int numBytes =*/ BASS_ChannelGetData(m_channel, ptr, len | BASS_DATA_FFT_REMOVEDC);

    }

    void MusicBass::GetWaveform(float *& ptr, size_t &length, size_t &channelCount, size_t &samplePerSec)
    {
        length = 0;
        ptr = nullptr;
        samplePerSec = 0;

        BASS_SAMPLE sampleInfo;
        if (!BASS_SampleGetInfo(m_sample, &sampleInfo))
        {
            LOGGER(Log::Logger().Error("Could not get sample info. BASS ERROR: %d", BASS_ErrorGetCode()));
            return;
        }

        const size_t tmpbuflen = sampleInfo.length;
        uint8_t *const tmpbuf = new uint8_t[tmpbuflen];

        if (BASS_SampleGetData(m_sample, tmpbuf))
        {
            length = m_length / m_bytesPerSample;
            channelCount = sampleInfo.chans;
            samplePerSec = m_samplePerSec;

            size_t buflen = length * channelCount;
            ptr = new float[buflen];

            if (sampleInfo.flags & BASS_SAMPLE_8BITS)
            {
                for (size_t i = 0; i < buflen; ++i)
                {
                    ptr[i] = tmpbuf[i] / 256.f;
                }
            }
            else if (sampleInfo.flags & BASS_SAMPLE_FLOAT)
            {
                memcpy(ptr, tmpbuf, buflen);
            }
            else // 16 bit per sample
            {
                uint16_t *const tmpbuf16 = reinterpret_cast<uint16_t *>(tmpbuf);
                for (size_t i = 0; i < buflen; ++i)
                {
                    ptr[i] = tmpbuf16[i] / 65536.f;
                }
            }
        }
        else {
            LOGGER(Log::Logger().Error("Could not read waveform data. BASS ERROR: %d", BASS_ErrorGetCode()));
        }

        delete[]tmpbuf;
    }
}



/// ====================================================================================================================================================
/// Factory class implementation
/// ====================================================================================================================================================
MusicBassLoader::MusicBassLoader(const std::string& sourceName) : IResourceBuilder(sourceName, sourceName)
{
}

MusicBassLoader::~MusicBassLoader()
= default;

void MusicBassLoader::Load(IResourceManager * const & resman, IResource * source)
{
    MusicResRef dest = dynamic_cast<MusicRes*>(source);
    if (dest.Invalid()) {
        THROW_EX(NullPointerException);
    }

    Music* music = new MusicBass();

    const IAssetRef asset = this->GetSourceAsset(resman);
    music->Initialize(asset);

    dest->AssingnRef(music);
}

IResource * MusicBassLoader::NewResource()
{
    return new MusicRes();
}

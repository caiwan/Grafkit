#pragma once

#include "core/Music.h"
#include "utils/Event.h"

namespace Idogep
{
    class Editor;

    class MusicProxy : public Grafkit::Music
    {
        friend class Editor;
    public:
        void Play() override;
        void Stop() override;
        void Pause(int e) override;
        void Update() override;

        void Initialize(Grafkit::StreamRef asset) override{}
        void Shutdown() override{}
        void ToggleMute() override;

        uint64_t GetSampleCount() override;
        uint64_t GetSampleCountPerSec() override;

        uint64_t GetTimeSample() override;
        void SetTimeSample(uint64_t t) override;
        void SetLoop(int e) override;
        int IsPlaying() override;

        void GetFFT(float* ptr, int segcount) override;
        void GetWaveform(float*& ptr, size_t& length, size_t& channelCount, size_t& samplePerSec) override;

        Event<> onMusicChanged;

        bool HasMusic() const;

    protected:
        Grafkit::MusicResRef m_music;
    };
}

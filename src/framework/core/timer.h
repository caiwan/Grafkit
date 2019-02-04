#pragma once 
#include "../stdafx.h"

namespace Grafkit {

    class Timer : virtual public Referencable {

    public:
        Timer() : m_bpm(0)
            , m_rows(0)
            , m_beatLenSample(0) {
        }

        virtual ~Timer() {
        }

        virtual void Play() = 0;
        virtual void Stop() = 0;
        virtual void Pause(int e) = 0;
        virtual void Update() = 0;

        virtual uint64_t GetSampleCount() = 0;
        virtual uint64_t GetSampleCountPerSec() = 0;

        virtual uint64_t GetTimeSample() = 0;
        virtual void SetTimeSample(uint64_t t) = 0;
        virtual void SetLoop(int e) = 0;
        virtual int IsPlaying() = 0;

        /* --- */
        double GetTime() { return double(GetTimeSample()) / double(GetSampleCountPerSec()); }
        double GetTimems() { return 1000.*GetTime(); }
        double GetTimeBeat() { return double(GetTimeSample()) / double(this->m_beatLenSample); }

        void SetTime(double t) { SetTimeSample(uint64_t(t * double(GetSampleCountPerSec()))); }
        void SetTimems(double t) { SetTimeSample(uint64_t(t * double(GetSampleCountPerSec()) / 1000.)); }
        void SetTimeBeat(double t) { SetTimeSample(uint64_t(t * double(m_beatLenSample))); }

        double GetBPM() const { return m_bpm; }
        void SetBPM(double f) { m_bpm = f; this->m_beatLenSample = uint64_t(m_bpm * double(GetSampleCountPerSec()) / 60.); }

        double GetLength() { return double(this->GetSampleCount()) / double(this->GetSampleCountPerSec()); }
        double GetLengthms() { return 1000.*double(this->GetSampleCount()) / double(this->GetSampleCountPerSec()); }

        void TogglePlay() { Pause(IsPlaying()); }

    private:
        double m_bpm;
        int m_rows;
        uint64_t m_beatLenSample;
    };

}

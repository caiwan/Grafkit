#pragma once 
#include "../stdafx.h"

namespace Grafkit {

	class Tracker : virtual public Referencable {

	public:
		Tracker() {
		}

		virtual ~Tracker() {
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
		double GetTime() { return (double)GetTimeSample() / (double)GetSampleCountPerSec(); }
		double GetTimems() { return 1000.*GetTime(); }
		double GetTimeBeat() { return (double)GetTimeSample() / (double)this->m_beatLenSample; }

		void SetTime(double t) { SetTimeSample((unsigned long)(t*(double)GetSampleCountPerSec())); }
		void SetTimems(double t) { SetTimeSample((unsigned long)(t*(double)GetSampleCountPerSec() / 1000.)); }
		void SetTimeBeat(double t) { SetTimeSample((unsigned long)(t*(double)m_beatLenSample)); }

		double GetBPM() { return m_bpm; }
		void SetBPM(double f) { m_bpm = f; this->m_beatLenSample = (unsigned int)((m_bpm / 60.) * (double)GetSampleCountPerSec()); }

		double GetLength() { return (double)this->GetSampleCount() / (double)this->GetSampleCountPerSec(); }
		double GetLengthms() { return 1000.*(double)this->GetSampleCount() / (double)this->GetSampleCountPerSec(); }

	private:
		double m_bpm;
		int m_rows;
		unsigned int m_beatLenSample;
	};

}

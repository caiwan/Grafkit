#pragma once

#include "core/Timer.h"
#include "core/Music.h"
#include "Event.h"

namespace Idogep {

	class Editor;

	class MusicProxy : public Grafkit::Timer
	{
		friend class Editor;
	public:
		virtual void Play();
		virtual void Stop();
		virtual void Pause(int e);
		virtual void Update();

		virtual uint64_t GetSampleCount();
		virtual uint64_t GetSampleCountPerSec();

		virtual uint64_t GetTimeSample();
		virtual void SetTimeSample(uint64_t t);
		virtual void SetLoop(int e);
		virtual int IsPlaying();

		void GetWaveform(float *&ptr, size_t &length, size_t &channelCount, size_t &samplePerSec);

		Event<> onMusicChanged;

	protected:
		Grafkit::MusicResRef m_music;
	};

}